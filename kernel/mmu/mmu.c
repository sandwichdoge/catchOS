#include "pageframe_alloc.h"
#include "paging.h"
#include "builddef.h"
#include "mmu.h"
#include "liballoc.h"
#include "utils/debug.h"

private void test_pageframe_firstpage() {
    void *p = pageframe_alloc(1);
    _dbg_log("Userland heap at: %x\n", p);
    pageframe_free(p, 1);
}

public void mmu_init() {
    paging_init();
    test_pageframe_firstpage();
}

public void* mmu_mmap(unsigned int size) {
    // return out + 0xc0000000;
    void* ret = _malloc(size);
    _dbg_log("[MMU]sbrk [%u], ret [0x%x]\n", size, ret);
    return ret;
}

public void mmu_munmap(void* mem) {
    _free(mem);
}

/*
[malloc()]
[mmap]    <- Userland, need to modify process' page tables.
[lib_malloc] <- library

[kmalloc] <- Kernel use, when allocated, add 0xc0000000, use kernel_page_directory.
*/