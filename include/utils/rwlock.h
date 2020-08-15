#ifndef INCLUDE_RWLOCK_H
#define INCLUDE_RWLOCK_H
#include "utils/atomic.h"
#include "utils/spinlock.h"

// Requires a counter semaphore and a binary semaphore.

// Local IRQs should be disabled before using rwlock.
struct rwlock {
    int sem_count;
    int reader_count;
    struct spinlock mtx;
    struct spinlock sem_mtx;
};

void rwlock_init(struct rwlock* lock);
void rwlock_read_acquire(struct rwlock* lock);
void rwlock_read_release(struct rwlock* lock);
void rwlock_write_acquire(struct rwlock* lock);
void rwlock_write_release(struct rwlock* lock);
#endif
