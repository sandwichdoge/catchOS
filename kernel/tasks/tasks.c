#include "tasks.h"
#include "timer.h"
#include "mmu.h"
#include "cpu_switch_to.h"
#include "builddef.h"
#include "utils/debug.h"

// Array of all current tasks. Only 64 tasks can run at a time for now.
private struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
private unsigned int _nr_tasks; // Current running tasks in the system.
struct task_struct kmain;       // Initial _current value, so we won't have to to if _current is NULL later.
struct task_struct* _current = &kmain;   // Current task that controls CPU.
struct task_struct* _scheduler;

// Read current EFLAGS register.
private 
unsigned int get_eflags() {
    unsigned int ret;
    asm("pushf\n"
    "movl (%%esp), %%eax\n"
    "popf\n"
    : "=a" (ret));
    return ret;
}

// When a task reaches return, it will call this task for cleanup.
private
void on_current_task_return_cb() {
    _current->state = TASK_JOINABLE;
    _tasks[_current->pid] = NULL;
    _nr_tasks--;
    task_yield();
}

public
struct task_struct* task_new(void (*fp)(void*), void* arg, unsigned int stack_size, int priority) {
    /*
    - Task's stack map -
    [arg here] - but how to return to cleanup func? must pop arg after using
    [Task's ret addr] - When task function reaches return. Should be addr of on_current_task_return_cb().
    [next EIP]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [reg]
    [EFLAGS reg] <- esp
    */
    //TODO _tasks[_nr_tasks]->cpu_state.esp = &on_current_task_return_cb;
    //on_task_exit -> TASK_JOINABLE, remove from _tasks array, task_yield().
    //no need for kmain after thread initialized (kmain will not get cpu time ever again after yielding)
    if (_nr_tasks == MAX_CONCURRENT_TASKS) return NULL; // Max number of tasks reached.
    int pid = -1;
    for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
        if (_tasks[i] == NULL) {
            pid = i;
        }
    }
    if (pid < 0) return NULL;

    _tasks[pid] = mmu_mmap(sizeof(struct task_struct));
    _tasks[pid]->stack_bottom = mmu_mmap(stack_size);
    _tasks[pid]->state = TASK_RUNNING;
    _dbg_log("Allocated TCB[%d]:[0x%x], stack top:[0x%x]\n", pid, _tasks[pid], _tasks[pid]->stack_bottom + stack_size);
    _tasks[pid]->cpu_state.esp = (unsigned int)_tasks[pid]->stack_bottom + stack_size;
    *(unsigned int*)(_tasks[pid]->cpu_state.esp) = (unsigned int)&on_current_task_return_cb;
    //_tasks[pid]->cpu_state.esp -= 4;
    //*(unsigned int*)(_tasks[pid]->cpu_state.esp) = (unsigned int)arg;
    _tasks[pid]->cpu_state.esp -= 36;
    *(unsigned int*)(_tasks[pid]->cpu_state.esp) = get_eflags();
    _tasks[pid]->pid = pid;
    _tasks[pid]->stack_state.eip = (unsigned int)fp;
    _tasks[pid]->interruptible = 1;
    _nr_tasks++;

    return _tasks[pid];
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
    cpu_switch_to(prev, next);
}

private
void* schedule(void* unused) {
    // Try round-robin first?
    asm("cli");

    static unsigned int i;
    _dbg_log("Total tasks: %u\n", _nr_tasks);
    _current->interruptible = 0;
    struct task_struct *next = NULL;
    while (1) {
        if (i >= MAX_CONCURRENT_TASKS) i = 0;
        next = _tasks[i];
        if (next) {
            break;
        } else {
            ++i;
        }
    }
    ++i;
    task_switch_to(next);
    _current->interruptible = 1;

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
unsigned int task_get_nr() {
    return _nr_tasks;
}

// Begin test section
struct task_struct *task1, *task2, *task3;

private void test_proc1(void *p) {
    while (1) {
        for (int i = 0; i < 2000000000; ++i) {
            _dbg_log("t1 %u:%d\n", getticks(), i);
            delay(200);
        }
    }
}

private void test_proc2(void *p) {
    while (1) {
        for (int i = 0; i < 2000000000; ++i) {
            _dbg_log("t2 %u:%d\n", getticks(), i);
            delay(200);
        }
    }
}

private void test_proc4(void* p) {
    _dbg_log("t4 [0x%x]\n", p);
}

private void test_proc3(void *p) {
    //struct task_struct *task4 = task_new(test_proc4, "abc\0", 1024, 1);
    p += 0x456;
    _dbg_log("t3 %u:%x\n", getticks(), (unsigned int)p);
    _dbg_break();
}

public void test_caller() {
    //task1 = task_new(test_proc1, NULL, 1024, 1);
    //task2 = task_new(test_proc2, NULL, 1024, 1);
    task3 = task_new(test_proc3, 0x123, 1024, 1);
    task_yield();
}
// End test section
