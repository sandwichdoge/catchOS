#ifndef INCLUDE_TASKS_H
#define INCLUDE_TASKS_H
#include "cpu_state.h"

enum TASK_STATE {TASK_READY = 0, TASK_WAITING, TASK_RUNNING};

struct task_struct {
    enum TASK_STATE state;
    int priority;
    struct cpu_state cpu_state;
    struct stack_state stack_state;
};

#endif
