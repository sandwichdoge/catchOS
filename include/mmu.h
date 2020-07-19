#ifndef INCLUDE_MALLOC_H
#define INCLUDE_MALLOC_H
void mmu_init();
void* mmu_mmap(unsigned int size);
void mmu_munmap(void* mem);
#endif
