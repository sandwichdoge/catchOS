#ifndef INCLUDE_TASKS_H
#define INCLUDE_TASKS_H
#include "cpu_state.h"
#include "stddef.h"

#define MAX_CONCURRENT_TASKS 64

/* 
READY = default, ready to run.
WAITING = in sleep().
RUNNING = being executed by cpu.
*/
enum TASK_STATE {TASK_READY = 0, TASK_WAITING, TASK_RUNNING};

struct task_struct {
    enum TASK_STATE state;
    unsigned int pid;
    int priority;
    int counter;        // How long current task has been running. Add to priority for real priority.
    struct cpu_state cpu_state;
    struct stack_state stack_state;
    void *stack_bottom; // Keep addr to free on task termination, and stack overflow detection.
    unsigned int stack_size;
    int interruptible;  // If non-zero then current task may not be interrupted (e.g. scheduler task)
};

struct task_struct* task_new(void (*fp)(void*), unsigned int stack_size, int priority);

#endif
