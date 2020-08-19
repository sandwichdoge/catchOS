#ifndef INCLUDE_MALLOC_H
#define INCLUDE_MALLOC_H
#include "stddef.h"
#include "stdint.h"
void mmu_init();
void* mmu_mmap(size_t size);
void mmu_munmap(void* mem);
void* mmu_mremap(void* old, size_t newsize);
#endif
