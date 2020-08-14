#ifndef INCLUDE_RWLOCK_H
#define INCLUDE_RWLOCK_H
#include "utils/atomic.h"
#include "utils/spinlock.h"

// Requires a counter semaphore and a binary semaphore.

struct rwlock {
    int sem_count;
    int reader_count;
    struct spinlock mtx;
    
};

#endif
