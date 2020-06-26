#ifndef INCLUDE_MALLOC_H
#define INCLUDE_MALLOC_H
void mmu_init();
void* mmu_sbrk(unsigned int size);
void mmu_free(void* mem);
#endif