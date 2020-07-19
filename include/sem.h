#ifndef INCLUDE_SEM_H
#define INCLUDE_SEM_H
#include "utils/list.h"

struct semaphore {
    int count;
    struct list_head* task_queue;
};

#endif
