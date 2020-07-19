#include "sem.h"
#include "builddef.h"
#include "utils/spinlock.h"
#include "mmu.h"
#include "tasks.h"

DECLARE_LOCK(semlock);

struct semaphore* sem_init(int count) {
    struct semaphore *ret = mmu_mmap(sizeof(*ret));
    ret->task_queue = NULL;
    ret->count = count;
    return ret;
}

void sem_uninit(struct semaphore* sem) {
    mmu_munmap(sem);
}

private
int cmpf(void* a, void* b) {
    return (a == b);
}

void sem_wait(struct semaphore* sem) {
    LOCK(semlock);
    int c = sem->count;
    UNLOCK(semlock);

    if (c > 0) {
        LOCK(semlock);
        sem->count--;
        UNLOCK(semlock);
    } else {    // Block
        // Push current task in queue.
        struct task_struct *curtask = task_get_current();

        LOCK(semlock);
        if (sem->task_queue == NULL) {  // Queue empty
            sem->task_queue = list_create(&curtask, sizeof(curtask));
        } else {
            list_insert_back(sem->task_queue, &curtask, sizeof(curtask));
        }
        UNLOCK(semlock);
        curtask->state = TASK_WAITING;
        task_yield();
    }
}

void sem_signal(struct semaphore* sem) {
    LOCK(semlock);
    
    UNLOCK(semlock);
}
