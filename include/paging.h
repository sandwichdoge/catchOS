#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H
void paging_init();
void paging_map(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir, unsigned int *page_table);
#endif
