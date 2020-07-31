#ifndef INCLUDE_PAGEFRAME_ALLOC_H
#define INCLUDE_PAGEFRAME_ALLOC_H
void* pageframe_alloc(unsigned int pages);
void pageframe_free(void* phys_addr, unsigned int pages);
// Manually reserve pages from a physical address. To make sure pageframe_alloc() will never return these pages later.
void pageframe_set_page_from_addr(void *phys_addr, unsigned int pages);
#endif
