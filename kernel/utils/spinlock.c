#include "utils/spinlock.h"
#include "utils/string.h"

static const int zero = 0;

inline void spinlock_init(struct spinlock* p) {
    _memset(p, 0, sizeof(*p));
}

inline void spinlock_lock(struct spinlock* p) {
    while (!__atomic_compare_exchange_n(&p->locked, &zero, 1, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
        ;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
}

inline void spinlock_unlock(struct spinlock* p) {
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    p->locked = 0;
}
