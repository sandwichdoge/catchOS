#include "mmu.h"
#include "interrupt.h"
#include "builddef.h"
#include "liballoc.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"

private
void test_pageframe_firstpage() {
    void* p = pageframe_alloc(1);
    _dbg_log("Userland heap at: %x\n", p);
    pageframe_free(p, 1);
}

private
void ISR_PAGEFAULT(unsigned int* return_reg, struct cpu_state* unused) {
    unsigned int faulting_address;
    asm volatile("movl %%cr2, %0" : "=r" (faulting_address));
    _dbg_log("Error. Pagefault at 0x[%x]!\n", faulting_address);
    _dbg_break();
}

public
void mmu_init() {
    interrupt_register(INT_PAGEFAULT, ISR_PAGEFAULT);
    paging_init();
    test_pageframe_firstpage();
}

public
void* mmu_mmap(unsigned int size) {
    // return out + 0xc0000000;
    void* ret = _malloc(size);
    _dbg_log("[MMU]Requested [%u], ret [0x%x]\n", size, ret);
    return ret;
}

public
void mmu_munmap(void* mem) { _free(mem); }

/*
[malloc()]
[mmap]    <- Userland, need to modify process' page tables.
[lib_malloc] <- library

[kmalloc] <- Kernel use, when allocated, add 0xc0000000, use kernel_page_directory.
*/
