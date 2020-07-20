#ifndef INCLUDE_SEM_H
#define INCLUDE_SEM_H
#include "utils/queue.h"
#include "utils/spinlock.h"

struct semaphore {
    int count;
    struct spinlock lock;
    struct queue task_queue;
};

void sem_init(struct semaphore* sem, int count);
void sem_wait(struct semaphore* sem);
void sem_signal(struct semaphore* sem);
#endif
