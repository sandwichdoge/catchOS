extern void loadPageDirectory(void* page_directory);
extern void enablePaging();

#define PAGE_TOTAL (32 * 1000 * 1024) / (4 * 1024) // Each page manages 4 KiB of phys memory

// Page index 123 in table index 456 will be mapped to (456 * 1024) + 123 = 467067. 467067 * 4 = 1868268 KiB.

unsigned int page_directory[1024] __attribute__((aligned(4096))); // 1024 page tables in page directory
unsigned int first_page_table[1024] __attribute__((aligned(4096))); // 1024 pages in a page table


void paging_init() {
    for (int i = 0; i < 1024; i++) {
        // Table is both read/write accessible
        page_directory[i] = 0x1000 | 0b11;
    }

    // Map 4 MB page table
    for(int i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes
        first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
    }

    // Set enable bit for directory
    page_directory[0] = ((unsigned int)first_page_table) | 3;

    loadPageDirectory((void*)page_directory);
    enablePaging();
}