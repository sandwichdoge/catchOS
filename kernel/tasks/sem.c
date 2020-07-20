#include "sem.h"
#include "builddef.h"
#include "utils/spinlock.h"
#include "utils/debug.h"
#include "mmu.h"
#include "tasks.h"

DECLARE_LOCK(semlock);

void sem_init(struct semaphore* sem, int count) {
    _memset(&sem->task_queue, 0, sizeof(sem->task_queue));
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
        queue_push(&sem->task_queue, &curtask, sizeof(curtask));
        _dbg_log("created head %x\n", sem->task_queue.tail);
        UNLOCK(semlock);
        curtask->state = TASK_WAITING;
        task_yield();
    }
}

void sem_signal(struct semaphore* sem) {
    // Update task_queue (pop head).
    LOCK(semlock);
    struct task_struct* popped_task;
    int is_queue_empty = queue_pop(&sem->task_queue, (void*)&popped_task, sizeof(popped_task));
    sem->count++;
    UNLOCK(semlock);

    // Change popped task state to TASK_RUNNING.
    if (!is_queue_empty) {
        popped_task->state = TASK_RUNNING;
    }
}
