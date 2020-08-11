#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H
#include "stddef.h"
#include "stdint.h"

// Divide these 2 and we get 1024 pages each PDE.
#define PAGE_SIZE 0x1000  // 4096

void paging_init();
void paging_map_table(size_t virtual_addr, size_t phys_addr, uint32_t *page_dir);
void paging_map_page(size_t virtual_addr, size_t phys_addr, uint32_t *page_dir);
uint32_t *get_kernel_pd();
#endif
