#include "tasks.h"

#include "builddef.h"
#include "cpu.h"
#include "cpu_switch_to.h"
#include "drivers/acpi/madt.h"
#include "drivers/lapic.h"
#include "drivers/pit.h"
#include "mmu.h"
#include "smp.h"
#include "timer.h"
#include "utils/maths.h"
#include "utils/atomic.h"
#include "utils/debug.h"
#include "utils/rwlock.h"
#include "utils/spinlock.h"
#include "utils/string.h"

// Array of all current tasks. Only 64 tasks can run at a time for now.
static struct task_struct* _tasks[MAX_CONCURRENT_TASKS];  // Task list.
static volatile int32_t _nr_tasks;                        // Current running tasks in the system.
struct task_struct** _current;                            // Current task that controls local CPU. TASK_STATE should always be TASK_RUNNING here.
struct task_struct** _bootstrap_tasks;                    // First task when a CPU starts up. CPU will never return to this task again.

struct rwlock lock_tasklist = {.sem_count = 1, .reader_count = 0, {0}, {0}};  // R/W lock for _tasks list.

private
void task_cleanup(struct task_struct* task) {
    mmu_munmap(task->stack_bottom);
    mmu_munmap(task);
}

// When a task reaches return, it will call this task for cleanup.
private
void on_current_task_return_cb() {
    struct task_struct* t = task_get_current();
    _dbg_log("On return pid [%u]\n", t->pid);
    t->interruptible = 0;

    rwlock_write_acquire(&lock_tasklist);
    _tasks[t->pid] = NULL;
    _nr_tasks--;
    rwlock_write_release(&lock_tasklist);

    if (t->state == TASK_RUNNING) {
        t->state = TASK_TERMINATED;
    } else {
        _dbg_log("Possible error: Invalid task state, expected TASK_RUNNING[%d] but got [%d]", TASK_RUNNING, t->state);
    }
    if (t->join_state == JOIN_DETACHED) {
        // Clean up if already detached, otherwise task_join() will clean up later from parent task.
        task_cleanup(t);
    }
    task_yield();
}

private
void task_startup(void (*fp)(void*), void* arg) {
    // Perform initialization before executing a task for the first time.
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
    if (atomic_compare_exchange(&_nr_tasks, MAX_CONCURRENT_TASKS, MAX_CONCURRENT_TASKS)) return NULL;  // Max number of tasks reached.
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

    // Set up new task's stack.
    *(size_t*)(newtask->cpu_state.esp) = (size_t)arg;
    newtask->cpu_state.esp -= sizeof(size_t);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)fp;
    newtask->cpu_state.esp -= sizeof(size_t);
    *(size_t*)(newtask->cpu_state.esp) = (size_t)&on_current_task_return_cb;
    newtask->cpu_state.esp -= (sizeof(struct cpu_state) + sizeof(size_t));
    *(size_t*)(newtask->cpu_state.esp) = get_flags_reg() | CPU_EFLAGS_IF;  // Always enable interrupt flag for new tasks.

    newtask->static_prio = priority;
    newtask->pid = pid;
    newtask->stack_state.eip = (size_t)task_startup;
    newtask->interruptible = 1;

    rwlock_write_acquire(&lock_tasklist);
    _tasks[pid] = newtask;
    _nr_tasks++;
    rwlock_write_release(&lock_tasklist);
    return newtask;
}

public
void task_join(struct task_struct* task) {
    while (task->state != TASK_TERMINATED) {
        task->dynamic_prio = 0;  // So scheduler will switch to a different task next time it's called.
        task_yield();
    }
    task_cleanup(task);
}

public
void task_detach(struct task_struct* task) {
    task->join_state = JOIN_DETACHED;
}

private
void task_switch_to(struct task_struct* next) {
    /*
    - Thread A enters function.
    - Context-switch happens and becomes Thread B.
    - Exit function as thread B.
    - Maybe switch page tables as well in the future?
    */
    //_current state is already TASK_READY here (changed by ISR TIMER).
    struct task_struct* current = task_get_current();
    if (current == next) return;

    struct task_struct* prev = current;
    task_set_current(next);
    //_dbg_log("[CPU%u]Switch to pid [%u]\n", smp_get_cpu_id(), next->pid);
    cpu_switch_to(prev, next);
}

private
void schedule(void* unused) {
    //_dbg_log("Total tasks:%u\n", _nr_tasks);

    task_get_current()->interruptible = 0;
    int c, next;
    for (;;) {
        c = -1;
        next = 0;
        rwlock_read_acquire(&lock_tasklist);
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct* t = _tasks[i];
            // if (t) _dbg_log("present task at [0x%x] %d, cnt:%d, state:%d\n", t, t->pid, t->counter, t->state);
            if (t && t->state == TASK_READY && t->dynamic_prio > c) {
                c = t->dynamic_prio;
                next = i;
            }
        }
        rwlock_read_release(&lock_tasklist);

        // Must lock to prevent 2 CPUs from picking the same next task.
        rwlock_write_acquire(&lock_tasklist);
        if (c > 0) {  // Found a suitable next task.
            _tasks[next]->state = TASK_RUNNING;
            rwlock_write_release(&lock_tasklist);
            break;
        }

        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct* t = _tasks[i];
            if (t && t->state == TASK_READY) {
                // The more iterations of the second for loop a task passes, the more its counter will be increased.
                // A task counter can never get larger than 2 * priority.
                t->dynamic_prio = (t->dynamic_prio >> 1) + t->static_prio;
            }
        }
        rwlock_write_release(&lock_tasklist);
    }
    task_switch_to(_tasks[next]);
    task_get_current()->interruptible = 1;
    // Old task has already become another task here.
    // Interrupt flag is also re-enabled in task_switch_to(). When switched back, ISR will return.
    // Then asm_int_handler_common() will change eip to the where previous task was interrupted by timer.
    return;
}

public
void task_yield() {
    // Switch control to scheduler, sched decides what process to continue.
    rwlock_write_acquire(&lock_tasklist);
    struct task_struct* current = task_get_current();
    if (current->state == TASK_RUNNING) {
        current->state = TASK_READY;
    }
    current->dynamic_prio = 0;
    rwlock_write_release(&lock_tasklist);
    schedule(NULL);
}

// Called by PIT ISR_TIMER.
public
void task_isr_tick() {
    struct task_struct* t = task_get_current();
    // Other processors may modify counter in schedule(). Need to lock.
    rwlock_write_acquire(&lock_tasklist);
    //_dbg_log("cpu%d isr\n", smp_get_cpu_id());
    t->dynamic_prio--;
    DEC_TIL_ZERO(t->sleep_avg);
    if (!t->interruptible || t->dynamic_prio > 0) {  // May not interrupt
        rwlock_write_release(&lock_tasklist);
        return;
    }
    t->dynamic_prio = 0;  // Guaranteed atomic

    if (t->state == TASK_RUNNING) {
        t->state = TASK_READY;
    }

    rwlock_write_release(&lock_tasklist);

    schedule(NULL);
}

private
void _cpu_idle_process(void* unused) {
    for (;;) {
        //_dbg_log("[cpu%d]idle\n", smp_get_cpu_id());
        asm("hlt");
    }
}

public
void tasks_init() {
    timer_init_sched(250);
    rwlock_init(&lock_tasklist);

    uint8_t cpu_count = smp_get_cpu_count();
    for (uint8_t i = 0; i < cpu_count; ++i) {
        _bootstrap_tasks[i] = mmu_mmap(sizeof(struct task_struct));
        _bootstrap_tasks[i]->pid = 99999;
        _bootstrap_tasks[i]->dynamic_prio = 1;
        _bootstrap_tasks[i]->state = TASK_RUNNING;
        _bootstrap_tasks[i]->interruptible = 1;
        _current[i] = _bootstrap_tasks[i];
        task_new(_cpu_idle_process, NULL, 0x600, 1);
    }
}

public
inline int32_t task_get_nr() {
    return atomic_load(&_nr_tasks);
}

public
inline struct task_struct* task_get_current() {
    return _current[smp_get_cpu_id()];
}

public
inline void task_set_current(struct task_struct* t) {
    _current[smp_get_cpu_id()] = t;
}

public
inline uint32_t task_getpid() {
    return task_get_current()->pid;
}
