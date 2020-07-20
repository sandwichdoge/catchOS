#include "sem.h"
#include "builddef.h"
#include "utils/debug.h"
#include "mmu.h"
#include "tasks.h"

void sem_init(struct semaphore* sem, int count) {
    _memset(&sem->task_queue, 0, sizeof(sem->task_queue));
    _memset(&sem->lock, 0, sizeof(sem->lock));
    sem->count = count;
}

void sem_wait(struct semaphore* sem) {
    spinlock_lock(&sem->lock);
    int c = sem->count;
    spinlock_unlock(&sem->lock);

    if (c > 0) {
        spinlock_lock(&sem->lock);
        sem->count--;
        spinlock_unlock(&sem->lock);
    } else {    // Block
        // Push current task in queue.
        struct task_struct *curtask = task_get_current();

        spinlock_lock(&sem->lock);
        queue_push(&sem->task_queue, &curtask, sizeof(curtask));
        _dbg_log("created head %x\n", sem->task_queue.tail);
        spinlock_unlock(&sem->lock);
        curtask->state = TASK_WAITING;
        task_yield();
    }
}

void sem_signal(struct semaphore* sem) {
    // Update task_queue (pop head).
    spinlock_lock(&sem->lock);
    struct task_struct* popped_task;
    int is_queue_empty = queue_pop(&sem->task_queue, (void*)&popped_task, sizeof(popped_task));
    sem->count++;
    spinlock_unlock(&sem->lock);

    // Change popped task state to TASK_RUNNING.
    if (!is_queue_empty) {
        popped_task->state = TASK_RUNNING;
    }
}
