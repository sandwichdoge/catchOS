#ifndef INCLUDE_PAGEFRAME_ALLOC_H
#define INCLUDE_PAGEFRAME_ALLOC_H
void pageframe_alloc_init();
void* pageframe_alloc(unsigned int pages);
void pageframe_free(void *phys_addr, unsigned int pages);
#endif