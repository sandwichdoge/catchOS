#ifndef INCLUDE_ATOMIC_H
#define INCLUDE_ATOMIC_H

//https://gcc.gnu.org/onlinedocs/gcc-4.8.2/gcc/_005f_005fatomic-Builtins.html

// Compares *addr with expected. If equal then writes desired into *addr, if not equal then 
// content in addr is written into &expected. Return result of this comparison.
int atomic_compare_exchange(int* addr, int expected, int desired);

void atomic_store(int* dst, int value);
int atomic_load(int* dst);

int atomic_fetch_add(int* addr, int n);
int atomic_fetch_sub(int* addr, int n);
#endif
