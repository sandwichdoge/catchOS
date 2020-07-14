#include "tasks.h"
#include "timer.h"
#include "mmu.h"
#include "cpu_switch_to.h"
#include "builddef.h"
#include "utils/debug.h"

// Array of all current tasks. Only 64 tasks can run at a time for now.
private struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
private unsigned int _nr_tasks; // Current running tasks in the system.
struct task_struct* _current;   // Current task that controls CPU.
struct task_struct* _scheduler;

public unsigned int task_get_nr() {
    return _nr_tasks;
}

public
struct task_struct* task_new(void (*fp)(void*), unsigned int stack_size, int priority) {
    //TODO _tasks[_nr_tasks]->cpu_state.esp = &on_current_task_exit_cb;
    //on_task_exit -> TASK_JOINABLE, remove from _tasks array, task_yield().
    //no need for kmain after thread initialized (kmain will not get cpu time ever again after yielding)
    if (_nr_tasks == MAX_CONCURRENT_TASKS) return NULL; // Max number of tasks reached.

    _tasks[_nr_tasks] = mmu_mmap(sizeof(struct task_struct));
    _tasks[_nr_tasks]->stack_bottom = mmu_mmap(stack_size);
    _tasks[_nr_tasks]->state = TASK_RUNNING;
    _dbg_log("Allocated TCB:[0x%x], stack top:[0x%x]\n", _tasks[_nr_tasks], _tasks[_nr_tasks]->stack_bottom + stack_size);
    _tasks[_nr_tasks]->cpu_state.esp = (unsigned int)_tasks[_nr_tasks]->stack_bottom + stack_size;
    _tasks[_nr_tasks]->cpu_state.esp -= 32;
    *(unsigned int*)(_tasks[_nr_tasks]->cpu_state.esp) = 0x00000286; // id vip vif ac vm rf nt IOPL=0 of df IF tf SF zf af PF cf
    _tasks[_nr_tasks]->pid = _nr_tasks + 1;
    _tasks[_nr_tasks]->stack_state.eip = (unsigned int)fp;
    _tasks[_nr_tasks]->interruptible = 1;
    _nr_tasks++;

    return _tasks[_nr_tasks - 1];
}

public
void task_join(struct task_struct* task) {
    while (task->state != TASK_JOINABLE) {
        task_yield();
    }
    mmu_munmap(task->stack_bottom);
    mmu_munmap(task);
    _nr_tasks--;
}

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
    //_dbg_log("[Switch]Prev:[0x%x], Next:[%u][0x%x]\n", prev, next->pid, next);
    cpu_switch_to(prev, next);
}

private
void* schedule(void* unused) {
    // Try round-robin first?
    //_dbg_log("Total tasks: %u\n", _nr_tasks);
    struct task_struct *next = NULL;
    while (1) {
        for (unsigned int i = 0; i < _nr_tasks; ++i) {
            if (_tasks[i] != _current) {
                next = _tasks[i];
                break;
            }
        }
        if (next) break;
    }
    task_switch_to(next);
    return NULL;
}

public
void task_yield() {
    // Switch control to scheduler, sched decides what process to continue.
    if (_current) _current->interruptible = 0;
    schedule(NULL);
    if (_current) _current->interruptible = 1;
}

// Begin test section
struct task_struct* task1;
struct task_struct* task2;

private void test_proc1(void *p) {
    while (1) {
        for (int i = 0; i < 2000000000; ++i) {
            _dbg_log("task1 %u:%d\n", getticks(), i);
            // BUG: stopped receiving timer interrupts unless sti() is called, probably lost interrupt on context switch, check EFLAGS?
            // Removing pushf in context switch fixes this -> try disabling global interrupts first before switch?
            delay(20);
            //task_switch_to(task2);
            task_yield();
        }
    }
}

private void test_proc2(void *p) {
    while (1) {
        for (int i = 0; i < 2000000000; ++i) {
            _dbg_log("task2 %u:%d\n", getticks(), i);
            delay(20);
            //task_switch_to(task1);
            task_yield();
        }
    }
}

public void test_caller() {
    task1 = task_new(test_proc1, 1024, 1);
    task2 = task_new(test_proc2, 1024, 1);
    //task_switch_to(task1);
    task_yield();
}
// End test section
