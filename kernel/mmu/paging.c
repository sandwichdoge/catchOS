#include "kinfo.h"
#include "kheap.h"
#include "utils/debug.h"
extern void loadPageDirectory(void* page_directory);
extern void enablePaging();

#define PAGE_SIZE 0x1000 // 4096
#define PDE_SIZE 0x400000

// Page index 123 in table index 456 will be mapped to (456 * 1024) + 123 = 467067. 467067 * 4 = 1868268 KiB.

unsigned int kernel_page_directory[1024] __attribute__((aligned(4096))); // 1024 page tables in page directory
char *pageframe_bitmap = (void*)0;

static char* pageframe_allocator_init() {
	struct kinfo *kinfo = get_kernel_info();
	// 1 bit represents 1 page in phys mem (4 KiB). Here we assign just enough for the bitmap.
	pageframe_bitmap = kmalloc_align(((kinfo->phys_mem_upper * 1024) / 4096) / 8, 4096);
}

static void pageframe_allocator_set_page(unsigned int page_no) {
	unsigned int byte_no = page_no / 8;
	unsigned int carry_bit = byte_no % 8;
	pageframe_bitmap[byte_no] = (1 << carry_bit);
}

static void pageframe_allocator_clear_page(unsigned int page_no) {
	unsigned int byte_no = page_no / 8;
	unsigned int carry_bit = byte_no % 8;
	pageframe_bitmap[byte_no] &= ~(1 << carry_bit);
}

static int pageframe_allocator_get_page(unsigned int page_no) {
	unsigned int byte_no = page_no / 8;
    unsigned int carry_bit = byte_no % 8;
    return ((pageframe_bitmap[byte_no] & (1 << carry_bit)) >> carry_bit);
}

static unsigned int virtual_addr_to_pde(unsigned int virtual_addr) {
	return virtual_addr >> 22;
}

// Map 1 page table (4MiB) from virtual address to phys_addr.
static void paging_map(unsigned int virtual_addr, unsigned int phys_addr, unsigned int *page_dir, unsigned int *page_table) {
	// Populate the page table. Fill each entry with corresponding physical address (increased by 0x1000 bytes each entry).
    for (unsigned int i = 0; i < 1024; i++) {
		// We can fit all addreses of 4GB physical memory into a PTE.
		// Since the page must be 4kB aligned, last 12 bits are always zeroes, so x86 uses them as access bits cleverly.
		page_table[i] = (phys_addr + (i * 0x1000)) | 3;
	}
	
	unsigned int pde = virtual_addr_to_pde(virtual_addr);
	page_dir[pde] = ((unsigned int)page_table - 0xc0000000) | 3;
}

void paging_init() {
	struct kinfo *kinfo = get_kernel_info();

	//unsigned int phys_mem_size_kb = (kinfo->phys_mem_upper - kinfo->phys_mem_lower); // e.g 31680
	unsigned int pages_total = kinfo->phys_mem_upper / 4;
	unsigned int page_tables_total = (pages_total + (1024 - 1)) / 1024; // Round up int division

	// Allocate memory for 1 page table
	unsigned int *page_tables = kmalloc_align(4096, 4096);

	// Map 1st page to 3GiB (kernel page)
    paging_map(0xc0000000, 0, kernel_page_directory, page_tables + 1024 * 0);

	unsigned int kernel_page_directory_phys = (unsigned int)kernel_page_directory - 0xc0000000;

	// loadPageDirectory() only accepts physical addresses.
    loadPageDirectory((void*)kernel_page_directory_phys);
    enablePaging();
}
