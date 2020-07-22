#include "builddef.h"
#include "kheap.h"
#include "kinfo.h"
#include "utils/debug.h"
extern void load_page_directory(void *page_directory);
extern void enable_paging();

// Divide these 2 and we get 1024 pages each PDE.
#define PAGE_SIZE 0x1000  // 4096

// A page directory can represent all 4GiB of memory, if it has 1024 entries.
// Page index 123 in table index 456 will be mapped to (456 * 1024) + 123 = 467067. 467067 * 4 = 1868268 KiB.

unsigned int kernel_page_directory[1024] __attribute__((aligned(4096)));  // 1024 page tables in page directory

private
unsigned int virtual_addr_to_pde(unsigned int virtual_addr) { return virtual_addr >> 22; }

// Map 1 page table (4MiB) from virtual address to phys_addr. Page table must persist in memory at all times.
public
void paging_map(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir, unsigned int *page_table) {
    _dbg_log("Mapping 0x%x to 0x%x\n", phys_addr, virtual_addr);

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
    // Allocate memory for 1 page table
    unsigned int *page_tables = kmalloc_align(4096, 4096);
    _dbg_log("[MMU]Page Table at 0x%x in kheap.\n", page_tables);

    // Map 1st page to 3GiB (kernel page)
    paging_map(0x0, 0, kernel_page_directory, page_tables + 1024 * 0);

    unsigned int kernel_page_directory_phys = (unsigned int)kernel_page_directory - 0x0;

    // load_page_directory() only accepts physical addresses.
    load_page_directory((void *)kernel_page_directory_phys);
    enable_paging();
}

public
unsigned int* get_kernel_pd() {
    return kernel_page_directory;
}
