#include "utils/spinlock.h"

inline void spinlock_lock(struct spinlock* p) {
    while (!__sync_bool_compare_and_swap(&p->locked, 0, 1))
        ;
    __sync_synchronize();
}

inline void spinlock_unlock(struct spinlock* p) {
    __sync_synchronize();
    p->locked = 0;
}
