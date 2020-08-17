#include "tasks.h"

#include "builddef.h"
#include "cpu_switch_to.h"
#include "drivers/pit.h"
#include "mmu.h"
#include "timer.h"
#include "utils/debug.h"
#include "utils/string.h"
#include "utils/rwlock.h"
#include "utils/spinlock.h"
#include "utils/atomic.h"
#include "cpu.h"

// Array of all current tasks. Only 64 tasks can run at a time for now.
static struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
static volatile int32_t _nr_tasks;                                      // Current running tasks in the system.
static struct task_struct kmaint = {.interruptible = 1, .counter = 1};  // Initial _current value, so we won't have to to if _current is NULL later.
struct task_struct* _current = &kmaint;                                 // Current task that controls local CPU. TASK_STATE should always be TASK_RUNNING here.

struct rwlock lock_tasklist;   // R/W lock for _tasks list.
struct spinlock lock_context_switch;

private
void task_cleanup(struct task_struct *task) {
    mmu_munmap(task->stack_bottom);
    mmu_munmap(task);
}

// When a task reaches return, it will call this task for cleanup.
private
void on_current_task_return_cb() {
    struct task_struct *t = _current;
    _dbg_log("On return pid [%u]\n", t->pid);

    rwlock_write_acquire(&lock_tasklist);
    _tasks[t->pid] = NULL;
    rwlock_write_release(&lock_tasklist);

    t->interruptible = 0;
    t->state = TASK_TERMINATED;
    if (t->join_state == JOIN_DETACHED) { 
        // Clean up if already detached, otherwise task_join() will clean up later from parent task.
        task_cleanup(t);
    }
    atomic_fetch_sub(&_nr_tasks, 1);
    task_yield();
}

private
void task_startup(void (*fp)(void*), void* arg) {
    // Context switch locks sl before switching, first time a task is switched to, it must unlock sl.
    // Because the first time CPU will jump to fp, not task_switch_to(), thus
    // => sl is not unlocked => deadlock on next context switch.
    spinlock_unlock(&lock_context_switch);
    fp(arg);
}

public
struct task_struct* task_new(void (*fp)(void*), void* arg, size_t stack_size, int32_t priority) {
    /*
    - Task's stack map -
    [arg]
    [fp]
    [task's ret addr] - When task function reaches return. Should be addr of on_current_task_return_cb().
    [next eip]        - Initial value should be addr of function task_startup(). This func will call fp after some init ritual.
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [EFLAGS reg] <- esp
    */
    if (atomic_compare_exchange(&_nr_tasks, MAX_CONCURRENT_TASKS, MAX_CONCURRENT_TASKS)) return NULL; // Max number of tasks reached.
    int pid = -1;
    rwlock_read_acquire(&lock_tasklist);
    for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
        if (_tasks[i] == NULL) {
            pid = i;
            break;
        }
    }
    rwlock_read_release(&lock_tasklist);
    if (pid < 0) return NULL;

    struct task_struct* newtask = NULL;
    newtask = mmu_mmap(sizeof(struct task_struct));
    _memset(newtask, 0, sizeof(*newtask));
    newtask->stack_bottom = mmu_mmap(stack_size);
    newtask->state = TASK_READY;
    newtask->join_state = JOIN_JOINABLE;
    newtask->cpu_state.esp = (size_t)newtask->stack_bottom + stack_size;
    _dbg_log("New task pid[%d]:[0x%x], stack top:[0x%x], stack size:[0x%x]\n", pid, newtask, newtask->cpu_state.esp, stack_size);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)arg;
    newtask->cpu_state.esp -= sizeof(size_t);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)fp;
    newtask->cpu_state.esp -= sizeof(size_t);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)&on_current_task_return_cb;
    newtask->cpu_state.esp -= (sizeof(struct cpu_state) + sizeof(size_t));
    *(size_t*)(newtask->cpu_state.esp) = get_flags_reg() | CPU_EFLAGS_IF;  // Always enable interrupt flag for new tasks.
    newtask->priority = priority;
    newtask->pid = pid;
    newtask->stack_state.eip = (size_t)task_startup;
    newtask->interruptible = 1;
    atomic_fetch_add(&_nr_tasks, 1);

    rwlock_write_acquire(&lock_tasklist);
    _tasks[pid] = newtask;
    rwlock_write_release(&lock_tasklist);
    return newtask;
}

public
void task_join(struct task_struct* task) {
    while (task->join_state != JOIN_JOINABLE) {
        task->counter = 1;  // So scheduler will switch to a different task next time it's called.
        task_yield();
    }
    task_cleanup(task);
}

public
void task_detach(struct task_struct* task) { task->join_state = JOIN_DETACHED; }

private
void task_switch_to(struct task_struct* next) {
    /*
    - Thread A enters function.
    - Context-switch happens and becomes Thread B.
    - Exit function as thread B.
    - Maybe switch page tables as well in the future?
    */
    if (_current == next) return;

    struct task_struct* prev = _current;
    _current = next;
    //_dbg_log("Switch to pid [%u]\n", next->pid);
    next->state = TASK_RUNNING;
    prev->state = TASK_READY;
    cpu_switch_to(prev, next);
}

private
void* schedule(void* unused) {
    //_dbg_log("Total tasks:%u\n", _nr_tasks);
    rwlock_write_acquire(&lock_tasklist);
    _current->interruptible = 0;
    rwlock_write_release(&lock_tasklist);
    int c, next;
    while (1) {
        c = -1;
        next = 0;
        rwlock_read_acquire(&lock_tasklist);
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct* t = _tasks[i];
            //if (t) _dbg_log("present task %d, cnt:%d, state:%d\n", t->pid, t->counter, t->state);
            if (t && (t->state == TASK_READY) && t->counter > c) {
                c = t->counter;
                next = i;
            }
        }
        rwlock_read_release(&lock_tasklist);
        if (c > 0) {
            //_dbg_log("cur=[%d], next=[%d], c=%d, IF=%d\n", _current->pid, next, c, get_flags_reg());
            break;
        }
        rwlock_write_acquire(&lock_tasklist);
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct* t = _tasks[i];
            if (t && t->state == TASK_READY) {
                // The more iterations of the second for loop a task passes, the more its counter will be increased.
                // A task counter can never get larger than 2 * priority.
                t->counter = (t->counter >> 1) + t->priority;
            }
        }
        rwlock_write_release(&lock_tasklist);
    }
    task_switch_to(_tasks[next]);
    _current->interruptible = 1;
    // Old task has already become another task here.
    // Interrupt flag is also re-enabled in task_switch_to(). When switched back, ISR will return.
    // Then asm_int_handler_common() will change eip to the where previous task was interrupted by timer.
    return NULL;
}

public
void task_yield() {
    // Switch control to scheduler, sched decides what process to continue.
    schedule(NULL);
}

// Called by PIT ISR_TIMER.
public
void task_isr_priority() {
    struct task_struct *t = _current;
    atomic_fetch_sub(&t->counter, 1);
    rwlock_read_acquire(&lock_tasklist);
    int may_not_interrupt = (t->counter > 0 || !t->interruptible);
    rwlock_read_release(&lock_tasklist);
    if (may_not_interrupt) {
        return;
    }
    rwlock_write_acquire(&lock_tasklist);
    t->state = TASK_READY;
    t->counter = 0;
    rwlock_write_release(&lock_tasklist);
    schedule(NULL);
}

public
inline int32_t task_get_nr() { return atomic_load(&_nr_tasks); }

public
inline struct task_struct* task_get_current() { return _current; }

public
inline uint32_t task_getpid() { return _current->pid; }

public
void tasks_init() {
    rwlock_init(&lock_tasklist);
    spinlock_init(&lock_context_switch);
}
