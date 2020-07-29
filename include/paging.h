#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H
// Divide these 2 and we get 1024 pages each PDE.
#define PAGE_SIZE 0x1000  // 4096

void paging_init();
void paging_map(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir);
void paging_map_page(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir);
unsigned int *get_kernel_pd();
#endif
