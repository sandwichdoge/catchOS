#ifndef INCLUDE_TASKS_H
#define INCLUDE_TASKS_H
#include "cpu_state.h"
#include "stddef.h"
#include "stdint.h"

#define MAX_CONCURRENT_TASKS 64

/*
READY = default, ready to run.
WAITING = in sleep().
RUNNING = being executed by cpu.
JOINABLE = function has returned, we may terminate task.
DETACHED = parent task no longer waits for this task.
*/
enum TASK_STATE { TASK_READY = 0, TASK_WAITING, TASK_RUNNING, TASK_TERMINATED };
enum JOIN_STATE { JOIN_JOINABLE, JOIN_DETACHED };

struct task_struct {
    enum TASK_STATE state;
    uint32_t pid;
    int32_t priority;
    int32_t counter;  // How long current task has been running. Add to priority for real priority.
    struct cpu_state cpu_state;
    struct stack_state stack_state;
    void* stack_bottom;  // Keep addr to free on task termination, and stack overflow detection.
    size_t stack_size;
    int32_t interruptible;  // If non-zero then current task will not be interrupted by timer ISR.
    enum JOIN_STATE join_state;
};

// Create a new task.
struct task_struct* task_new(void (*fp)(void*), void* arg, size_t stack_size, int32_t priority);

// Join a running task, release its resources.
void task_join(struct task_struct*);

// Detach a running task.
void task_detach(struct task_struct*);

// Give up control to another process.
void task_yield();

void test_caller();

// Get number of running tasks.
int32_t task_get_nr();

// Called by PIT INT_SYSTIMER.
void task_isr_priority();

// Get current running task.
struct task_struct* task_get_current();

// Get current running task's pid.
uint32_t task_getpid();

void tasks_init();
#endif
