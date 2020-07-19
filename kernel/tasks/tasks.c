#include "tasks.h"
#include "timer.h"
#include "mmu.h"
#include "cpu_switch_to.h"
#include "builddef.h"
#include "utils/debug.h"

#define EFLAGS_IF (1 << 9)

// Array of all current tasks. Only 64 tasks can run at a time for now.
private struct task_struct* _tasks[MAX_CONCURRENT_TASKS];
private unsigned int _nr_tasks; // Current running tasks in the system.
struct task_struct kmaint = {.interruptible = 0};    // Initial _current value, so we won't have to to if _current is NULL later.
struct task_struct* _current = &kmaint;   // Current task that controls CPU.
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
    _dbg_log("On return pid [%u]", _current->pid);
    _current->interruptible = 0;
    _tasks[_current->pid] = NULL;
    if (_current->state == TASK_DETACHED) {
        mmu_munmap(_current->stack_bottom);
        mmu_munmap(_current);
    } else {
        _current->state = TASK_JOINABLE;
    }
    _current->interruptible = 1;
    _nr_tasks--;
    task_yield();
}

public
struct task_struct* task_new(void (*fp)(void*), void* arg, unsigned int stack_size, int priority) {
    /*
    - Task's stack map -
    [arg]
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
    if (_nr_tasks == MAX_CONCURRENT_TASKS) return NULL; // Max number of tasks reached.
    _current->interruptible = 0;
    int pid = -1;
    for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
        if (_tasks[i] == NULL) {
            pid = i;
            break;
        }
    }
    if (pid < 0) return NULL;

    struct task_struct *newtask = NULL;
    newtask = mmu_mmap(sizeof(struct task_struct));
    newtask->stack_bottom = mmu_mmap(stack_size);
    newtask->state = TASK_RUNNING;
    newtask->cpu_state.esp = (unsigned int)newtask->stack_bottom + stack_size;
    _dbg_log("Allocated TCB[%d]:[0x%x], stack top:[0x%x]\n", pid, newtask, newtask->cpu_state.esp);
    *(unsigned int*)(newtask->cpu_state.esp) = (unsigned int)arg;
    newtask->cpu_state.esp -= 4;
    *(unsigned int*)(newtask->cpu_state.esp) = (unsigned int)&on_current_task_return_cb;
    newtask->cpu_state.esp -= 36;
    *(unsigned int*)(newtask->cpu_state.esp) = get_eflags() | EFLAGS_IF; // Always enable interrupt flag for new tasks.
    newtask->priority = priority;
    newtask->pid = pid;
    newtask->stack_state.eip = (unsigned int)fp;
    newtask->interruptible = 1;
    _nr_tasks++;

    _tasks[pid] = newtask;
    _current->interruptible = 1;
    return newtask;
}

public
void task_join(struct task_struct* task) {
    while (task->state != TASK_JOINABLE) {
        task->counter = 1;
        task_yield();
    }
    mmu_munmap(task->stack_bottom);
    mmu_munmap(task);
}

public
void task_detach(struct task_struct* task) {
    task->state = TASK_DETACHED;
}

private
void task_switch_to(struct task_struct* next) {
    /*
    - Thread A enters function.
    - Context-switch happens and becomes Thread B.
    - Exit function as thread B.
    - Maybe switch page tables as well in the future?
    */
    //_dbg_log("Switch from task 0x%x to task 0x%x\n", _current, next);
    if (_current == next) return;
    struct task_struct* prev = _current;
    _current = next;
    cpu_switch_to(prev, next);
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
            struct task_struct *t = _tasks[i];
            if (t && (t->state == TASK_RUNNING || t->state == TASK_DETACHED) && t->counter > c) {
                c = t->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++i) {
            struct task_struct *t = _tasks[i];
            if (t) {
                // The more iterations of the second for loop a task passes, the more its counter will be increased.
                // A task counter can never get larger than 2 * priority.
                t->counter = (t->counter >> 1) + t->priority;
            }
        }
    }
    task_switch_to(_tasks[next]);

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
    //_dbg_log("isr\n");
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
    for (int i = 0; i < 200; ++i) {
        _dbg_log("t4 [0x%x][%d]\n", p, i);
        delay(20);
    }
}

private void test_proc3(void *p) {
    struct task_struct *task4 = task_new(test_proc4, (void*)0x777, 1024, 1);
    _dbg_log("t3 %u:%x\n", getticks(), p);
    task_join(task4);
    _dbg_log("joined task4\n");
}

public void test_caller() {
    task1 = task_new(test_proc1, NULL, 1024, 1);
    task2 = task_new(test_proc2, NULL, 1024, 1);
    task3 = task_new(test_proc3, (void*)0x123, 1024, 1);
    task_yield();
}
// End test section
