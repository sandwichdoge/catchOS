#ifndef INCLUDE_SEM_H
#define INCLUDE_SEM_H
#include "stdint.h"
#include "utils/queue.h"
#include "utils/spinlock.h"

struct semaphore {
    int32_t count;
    struct spinlock lock;
    struct queue task_queue;
};

void sem_init(struct semaphore* sem, int32_t count);
void sem_wait(struct semaphore* sem);
void sem_signal(struct semaphore* sem);
#endif
