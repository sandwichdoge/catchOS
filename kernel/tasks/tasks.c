#include "tasks.h"
#include "timer.h"
#include "mmu.h"
#include "builddef.h"
#include "utils/debug.h"

// Array of all current tasks. Only 64 tasks can run at a time for now.
private struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
private unsigned int _nr_tasks; // Current running tasks in the system.

private void test_proc(void *p) {
    for (int i = 0; i < 4; ++i) {
        delay(1000);
        _dbg_log("%d", i);
    }
}

public
struct task_struct* task_new(void (*fp)(void*), unsigned int stack_size, int priority) {
    if (_nr_tasks == MAX_CONCURRENT_TASKS) return NULL; // Max number of tasks reached.

    _tasks[_nr_tasks] = (struct task_struct*)mmu_mmap(sizeof(struct task_struct));
    _tasks[_nr_tasks]->stack_bottom = mmu_mmap(stack_size);
    _tasks[_nr_tasks]->cpu_state.esp = (unsigned int)_tasks[_nr_tasks]->stack_bottom + stack_size;
    _tasks[_nr_tasks]->state = TASK_RUNNING;
    _tasks[_nr_tasks]->pid = _nr_tasks;
    _nr_tasks++;
    _tasks[_nr_tasks]->stack_state.eip = (unsigned int)fp;    // Instruction pointer

    return NULL;
}

public
void task_yield(struct task_struct* task) {
    // TODO: Switch control to scheduler, sched decides what process to continue.
}

public
void task_join(struct task_struct* task) {
    while (task->state != TASK_JOINABLE) {
        task_yield(task);
    }
    mmu_munmap(task->stack_bottom);
    mmu_munmap(task);
    _nr_tasks--;
}
