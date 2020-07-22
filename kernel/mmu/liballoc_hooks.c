#include "kheap.h"
#include "liballoc.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"
#include "utils/spinlock.h"

#define PDE_SIZE 0x400000
extern unsigned int kernel_page_directory[1024];

static struct spinlock mmulock = {0};
int liballoc_lock() {
    spinlock_lock(&mmulock);
    return 0;
}

int liballoc_unlock() {
    spinlock_unlock(&mmulock);
    return 0;
}

void* pageframe_alloc_liballoc(unsigned int pages) {
    void* ret = pageframe_alloc(pages);
    if (ret) {
        // 1-1 map
        unsigned int* page_tables = kmalloc_align(4096 * pages, 4096);
        for (unsigned int i = 0; i < pages; ++i) {
            paging_map((unsigned int)ret + 0x0 + i * PDE_SIZE, (unsigned int)ret + i * PDE_SIZE, kernel_page_directory, page_tables + 1024 * i);
        }
        ret = (char*)ret + 0x0;
    } else {  // Out of memory
        _dbg_log("[MMU]Out of pages.\n");
    }
    return ret;
}

void pageframe_free_liballoc(void* virt_addr, unsigned int pages) {
    if (virt_addr) {
        virt_addr = (char*)virt_addr - 0x0;
    }
    pageframe_free(virt_addr, pages);
}

void* liballoc_alloc(size_t pages) { return pageframe_alloc_liballoc((unsigned int)pages); }

int liballoc_free(void* p, size_t pages) {
    pageframe_free_liballoc(p, (unsigned int)pages);
    return 0;
}
