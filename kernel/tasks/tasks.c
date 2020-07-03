#include "tasks.h"
#include "timer.h"
#include "mmu.h"
#include "builddef.h"
#include "utils/debug.h"

// Array of all current tasks. Only 64 tasks can run at a time for now.
private struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
private unsigned int _nr_tasks; // Current running tasks in the system.

private void test_proc(void *p) {
    int n = 0;
    for (int i = 0; i < 4; ++i) {
        delay(1000);
        _dbg_log("%d", i);
    }
}

public
struct task_struct* task_new(void (*fp)(void*), unsigned int stack_size, int priority) {
    _tasks[_nr_tasks] = (struct task_struct*)mmu_mmap(sizeof(struct task_struct));
    _tasks[_nr_tasks]->stack_bottom = mmu_mmap(stack_size);
    _tasks[_nr_tasks]->cpu_state.esp = (char*)_tasks[_nr_tasks]->stack_bottom + stack_size;
    _tasks[_nr_tasks]->state = TASK_RUNNING;
    _tasks[_nr_tasks]->pid = _nr_tasks++;
    _tasks[_nr_tasks]->stack_state.eip = fp;    // Instruction pointer

    return NULL;
}
