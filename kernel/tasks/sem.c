#include "sem.h"
#include "builddef.h"
#include "utils/spinlock.h"
#include "utils/debug.h"
#include "mmu.h"
#include "tasks.h"

DECLARE_LOCK(semlock);

void sem_init(struct semaphore* sem, int count) {
    sem->task_queue = NULL;
    sem->count = count;
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
            _dbg_log("created head %x\n", sem->task_queue);
        } else {
            list_insert_back(sem->task_queue, &curtask, sizeof(curtask));
        }
        // EXPECT curtask == sem->task_queue->data
        UNLOCK(semlock);
        curtask->state = TASK_WAITING;
        task_yield();
    }
}

void sem_signal(struct semaphore* sem) {
    LOCK(semlock);
    // Update task_queue (pop head).
    struct list_head *oldhead = sem->task_queue;
    if (oldhead) {
        struct list_head *newhead = oldhead->next;
        sem->task_queue = newhead;
    } 
    sem->count++;
    UNLOCK(semlock);

    // Change popped task state to TASK_RUNNING.
    if (oldhead) {
        struct task_struct* t = *(struct task_struct**)oldhead->data;
        t->state = TASK_RUNNING;

        // Free old head of task_queue.
        list_remove(oldhead, oldhead);
        _dbg_log("removed head %x\n", oldhead);
    }
}
