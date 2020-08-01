#ifndef INCLUDE_MALLOC_H
#define INCLUDE_MALLOC_H
#include "stddef.h"
void mmu_init();
void* mmu_mmap(size_t size);
void mmu_munmap(void* mem);
#endif
