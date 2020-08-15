#include "utils/atomic.h"

inline int atomic_compare_exchange(int* addr, int expected, int desired) {
    return __atomic_compare_exchange_n(addr, &expected, desired, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

// *dst = value;
inline void atomic_store(int* dst, int value) {
    __atomic_store_n(dst, 0, __ATOMIC_SEQ_CST);
}

inline int atomic_load(int* src) {
    return __atomic_load_n(src, __ATOMIC_SEQ_CST);
}

// { tmp = *ptr; *ptr += val; return tmp; }
inline int atomic_fetch_add(int* addr, int n) {
    return __atomic_fetch_add(addr, n, __ATOMIC_SEQ_CST);
}

// { tmp = *ptr; *ptr -= val; return tmp; }
inline int atomic_fetch_sub(int* addr, int n) {
    return __atomic_fetch_sub(addr, n, __ATOMIC_SEQ_CST);
}
