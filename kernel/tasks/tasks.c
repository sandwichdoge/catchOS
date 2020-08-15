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

#define EFLAGS_IF (1 << 9)

// Array of all current tasks. Only 64 tasks can run at a time for now.
static struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
static uint32_t _nr_tasks;                                              // Current running tasks in the system.
static struct task_struct kmaint = {.interruptible = 0, .counter = 1};  // Initial _current value, so we won't have to to if _current is NULL later.
struct task_struct* _current = &kmaint;                                 // Current task that controls local CPU. TASK_STATE should always be TASK_RUNNING here.

struct rwlock lock_tasks;   // R/W lock for _tasks list.
struct spinlock lock_sched;

// Read current EFLAGS register.
private
size_t get_flags_reg() {
    size_t ret;
    asm("pushf\n"
        "movl (%%esp), %%eax\n"
        "popf\n"
        : "=a"(ret));
    return ret;
}

// When a task reaches return, it will call this task for cleanup.
private
void on_current_task_return_cb() {
    _dbg_log("On return pid [%u]\n", _current->pid);
    _current->interruptible = 0;
    _current->state = TASK_TERMINATED;
    _tasks[_current->pid] = NULL;
    if (_current->join_state == JOIN_DETACHED) {
        mmu_munmap(_current->stack_bottom);
        mmu_munmap(_current);
    }
    _nr_tasks--;
    _current->interruptible = 1;
    task_yield();
}

private
void task_startup(void (*fp)(void*), void* arg) {
    _dbg_log("task_startup()\n");
    spinlock_unlock(&lock_sched);
    fp(arg);
}

public
struct task_struct* task_new(void (*fp)(void*), void* arg, size_t stack_size, int32_t priority) {
    /*
    - Task's stack map -
    [arg]
    [fp]
    [task's ret addr] - When task function reaches return. Should be addr of on_current_task_return_cb().
    [next eip]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [EFLAGS reg] <- esp
    */
    if (_nr_tasks == MAX_CONCURRENT_TASKS) return NULL;  // Max number of tasks reached.
    _current->interruptible = 0;
    int pid = -1;
    for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
        if (_tasks[i] == NULL) {
            pid = i;
            break;
        }
    }
    if (pid < 0) return NULL;

    struct task_struct* newtask = NULL;
    newtask = mmu_mmap(sizeof(struct task_struct));
    _memset(newtask, 0, sizeof(*newtask));
    newtask->stack_bottom = mmu_mmap(stack_size);
    newtask->state = TASK_READY;
    newtask->join_state = JOIN_JOINABLE;
    newtask->cpu_state.esp = (size_t)newtask->stack_bottom + stack_size;
    _dbg_log("Allocated TCB[%d]:[0x%x], stack top:[0x%x], stack size:[0x%x]\n", pid, newtask, newtask->cpu_state.esp, stack_size);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)arg;
    newtask->cpu_state.esp -= sizeof(size_t);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)fp;
    newtask->cpu_state.esp -= sizeof(size_t);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)&on_current_task_return_cb;
    newtask->cpu_state.esp -= (sizeof(struct cpu_state) + sizeof(size_t));
    *(size_t*)(newtask->cpu_state.esp) = get_flags_reg() | EFLAGS_IF;  // Always enable interrupt flag for new tasks.
    newtask->priority = priority;
    newtask->pid = pid;
    newtask->stack_state.eip = (size_t)task_startup;
    newtask->interruptible = 1;
    _nr_tasks++;

    _tasks[pid] = newtask;
    _current->interruptible = 1;
    return newtask;
}

public
void task_join(struct task_struct* task) {
    while (task->join_state != JOIN_JOINABLE) {
        task->counter = 1;  // So scheduler will switch to a different task next time it's called.
        task_yield();
    }
    mmu_munmap(task->stack_bottom);
    mmu_munmap(task);
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
    spinlock_lock(&lock_sched);
    next->state = TASK_RUNNING;
    prev->state = TASK_READY;
    cpu_switch_to(prev, next);  // If task was just initialized, we'll jump to func pointer, not here. Need to unlock in func?
    spinlock_unlock(&lock_sched);
}

private
void* schedule(void* unused) {
    asm("cli");
    //_dbg_log("Total tasks:%u\n", _nr_tasks);

    int c, next;
    while (1) {
        c = -1;
        next = 0;
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct* t = _tasks[i];
            if (t && (t->state == TASK_READY) && t->counter > c) {
                c = t->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct* t = _tasks[i];
            if (t && t->state == TASK_READY) {
                // The more iterations of the second for loop a task passes, the more its counter will be increased.
                // A task counter can never get larger than 2 * priority.
                t->counter = (t->counter >> 1) + t->priority;
            }
        }
    }
    task_switch_to(_tasks[next]);
    // Old task has already become another task here.
    // Interrupt flag is also re-enabled in task_switch_to(). When switched back, ISR will return.
    // Then asm_int_handler_common() will change eip to the where previous task was interrupted by timer.

    asm("sti");
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
    _current->counter--;
    if (_current->counter > 0 || !_current->interruptible) {
        return;
    }
    _current->counter = 0;
    schedule(NULL);
}

public
uint32_t task_get_nr() { return _nr_tasks; }

public
inline struct task_struct* task_get_current() { return _current; }

public
inline uint32_t task_getpid() { return _current->pid; }

public
void tasks_init() {
    rwlock_init(&lock_tasks);
    spinlock_init(&lock_sched);
}
