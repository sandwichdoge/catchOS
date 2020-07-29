#include "builddef.h"
#include "paging.h"
#include "kheap.h"
#include "kinfo.h"
#include "utils/debug.h"
extern void load_page_directory(void *page_directory);
extern void enable_paging();

// A page directory can represent all 4GiB of memory, if it has 1024 entries.
// Page index 123 in table index 456 will be mapped to (456 * 1024) + 123 = 467067. 467067 * 4 = 1868268 KiB.

unsigned int kernel_page_directory[1024] __attribute__((aligned(4096)));  // 1024 page tables in page directory
static unsigned int* allocated_page_tables[1024];

private
unsigned int virtual_addr_to_pde(unsigned int virtual_addr) { return virtual_addr >> 22; }

private
unsigned int* get_page_table(unsigned int phys_addr) {
    // Divide by 0x400000 for index
    unsigned int index = phys_addr / 0x400000;

    // See if a pagetable for this address already exists. If not then allocate, otherwise return it.
    if (!allocated_page_tables[index]) {
        allocated_page_tables[index] = kmalloc_align(4096, 4096);
    }
    return allocated_page_tables[index];
}

// Map 1 page (4096 bytes)
public
void paging_map_page(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir) {
    unsigned int *page_table = get_page_table(phys_addr);
    _dbg_log("Mapping 1 page 0x%x to 0x%x, kernel_page_dir[0x%x], page_table[0x%x]\n", phys_addr, virtual_addr, page_dir, page_table);
    unsigned int pte = ((virtual_addr % 0x400000) / 0x1000);

    page_table[pte] = (phys_addr + (pte * 0x1000)) | 3;

    unsigned int pde = virtual_addr_to_pde(virtual_addr);
    page_dir[pde] = ((unsigned int)page_table - 0x0) | 3;
}

// Map 1 page table (4MiB) from virtual address to phys_addr. Page table must persist in memory at all times.
public
void paging_map_table(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir) {
    unsigned int *page_table = get_page_table(phys_addr);
    _dbg_log("Mapping 1 table 0x%x to 0x%x, kernel_page_dir[0x%x], page_table[0x%x]\n", phys_addr, virtual_addr, page_dir, page_table);

    // Populate the page table. Fill each entry with corresponding physical address (increased by 0x1000 bytes each entry).
    for (unsigned int i = 0; i < 1024; i++) {
        // A PTE can contain any address of 4GB physical memory.
        // Since the page must be 4kB aligned, last 12 bits are always zeroes, x86 uses them as access bits cleverly.
        page_table[i] = (phys_addr + (i * 0x1000)) | 3;
    }

    unsigned int pde = virtual_addr_to_pde(virtual_addr);
    page_dir[pde] = ((unsigned int)page_table - 0x0) | 3;
}

// We're already in high-half kernel after kboot. So all addresses below are virtual.
public
void paging_init() {
    // Allocate memory for 2 page table
    //unsigned int *page_tables = kmalloc_align(4096 * 2, 4096);

    // Map two 1st page tables to 3GiB (kernel page)
    for (int i = 0; i < 2; ++i) {
        paging_map_table(0x0 + 0x400000 * i, 0x400000 * i, kernel_page_directory);
    }

    unsigned int kernel_page_directory_phys = (unsigned int)kernel_page_directory - 0x0;

    // load_page_directory() only accepts physical addresses.
    load_page_directory((void *)kernel_page_directory_phys);
    enable_paging();
}

public
unsigned int *get_kernel_pd() { return kernel_page_directory; }
