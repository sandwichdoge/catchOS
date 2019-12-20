extern void loadPageDirectory(void* page_directory);
extern void enablePaging();

#define PAGE_TOTAL (32 * 1000 * 1024) / (4 * 1024) // Each page manages 4 KiB of phys memory

// Page index 123 in table index 456 will be mapped to (456 * 1024) + 123 = 467067. 467067 * 4 = 1868268 KiB.

unsigned int page_directory[PAGE_TOTAL] __attribute__((aligned(4096))); // 1024 page tables in page directory
//unsigned int first_page_table[1024] __attribute__((aligned(4096))); // 1024 pages in a page table
unsigned int *page_table_list[PAGE_TOTAL] __attribute__((aligned(4096)));

void paging_init() {
    for (int i = 0; i < PAGE_TOTAL; i++) {
        // Table is both read/write accessible
        page_directory[i] = 0x1000 | 0b11;
    }

    // Map 1024 page tables, each manages 4MB.
    for(int i = 0; i < PAGE_TOTAL; i++) {
        for (int j = 0; j < 1024; j++) {
            page_table_list[i][j] = (j * 0x1000) | 3;
        }
    }

    // Set enable bit for directory
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = ((unsigned int)page_table_list[i]) | 3;
    }

    loadPageDirectory((void*)page_directory);
    enablePaging();
}