#ifndef INCLUDE_PAGEFRAME_ALLOC_H
#define INCLUDE_PAGEFRAME_ALLOC_H
#include "stddef.h"
#include "stdint.h"
void* pageframe_alloc(uint32_t pages);
void pageframe_free(void* phys_addr, uint32_t pages);
// Manually reserve pages from a physical address. To make sure pageframe_alloc() will never return these pages later.
void pageframe_set_page_from_addr(void* phys_addr, uint32_t pages);
uint32_t page_from_addr(size_t addr);
#endif
