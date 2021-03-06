#include "sem.h"

#include "builddef.h"
#include "tasks.h"
#include "utils/string.h"

public
void sem_init(struct semaphore* sem, int32_t count) {
    _memset(&sem->task_queue, 0, sizeof(sem->task_queue));
    spinlock_init(&sem->lock);
    sem->count = count;
}

public
void sem_wait(struct semaphore* sem) {
    spinlock_lock(&sem->lock);
    int32_t c = sem->count;
    spinlock_unlock(&sem->lock);

    if (c > 0) {
        spinlock_lock(&sem->lock);
        sem->count--;
        spinlock_unlock(&sem->lock);
    } else {  // Block
        // Push current task in queue.
        struct task_struct* curtask = task_get_current();

        spinlock_lock(&sem->lock);
        queue_push(&sem->task_queue, &curtask, sizeof(curtask));
        spinlock_unlock(&sem->lock);
        curtask->state = TASK_WAITING;
        task_yield();
    }
}

public
void sem_signal(struct semaphore* sem) {
    // Update task_queue (pop head).
    struct task_struct* popped_task;
    spinlock_lock(&sem->lock);
    int32_t is_queue_empty = queue_pop(&sem->task_queue, &popped_task, sizeof(popped_task));
    sem->count++;
    spinlock_unlock(&sem->lock);

    // Change popped task state to TASK_READY again, so scheduler can choose it.
    if (!is_queue_empty) {
        popped_task->state = TASK_READY;
    }
}
