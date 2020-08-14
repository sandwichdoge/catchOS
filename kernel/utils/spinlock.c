#include "utils/spinlock.h"
#include "utils/string.h"
#include "utils/atomic.h"

inline void spinlock_init(struct spinlock* p) {
    _memset(p, 0, sizeof(*p));
}

inline void spinlock_lock(struct spinlock* p) {
    while (!atomic_compare_exchange(&p->locked, 0, 1))
        ;
}

inline void spinlock_unlock(struct spinlock* p) {
    atomic_store(&p->locked, 0);
}
