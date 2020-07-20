#ifndef INCLUDE_SEM_H
#define INCLUDE_SEM_H
#include "utils/list.h"

struct semaphore {
    int count;
    struct list_head* task_queue;
};

void sem_init(struct semaphore* sem, int count);
void sem_wait(struct semaphore* sem);
void sem_signal(struct semaphore* sem);
#endif
