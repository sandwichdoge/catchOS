#include "builddef.h"
#include "interrupt.h"
#include "liballoc.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"

#ifndef TARGET_HOST
private
void test_pageframe_firstpage() {
    void* p = pageframe_alloc(1);
    _dbg_log("Userland heap at: %x\n", p);
    pageframe_free(p, 1);
}

private
void ISR_PAGEFAULT(size_t* return_reg, struct cpu_state* unused) {
    size_t faulting_address;
    asm volatile("movl %%cr2, %0" : "=r"(faulting_address));
    _dbg_log("Error. Pagefault access [0x%x]!\n", faulting_address);
    _dbg_screen("Error. Pagefault access [0x%x]!\n", faulting_address);
    _dbg_break();
}

public
void mmu_init() {
    interrupt_register(INT_PAGEFAULT, ISR_PAGEFAULT);
    paging_init();
    test_pageframe_firstpage();
}
#else
#include <stdlib.h>
#endif  // TARGET_HOST

public
void* mmu_mmap(size_t size) {
#ifdef TARGET_HOST
    void* ret = malloc(size);
#else
    void* ret = _malloc(size);
#endif
    _dbg_log("[MMU]Requested [%u], ret [0x%x]\n", size, ret);
    return ret;
}

public
void* mmu_mremap(void* old, size_t newsize) {
#ifdef TARGET_HOST
    void* ret = realloc(old, newsize);
#else
    void* ret = _realloc(old, newsize);
#endif
    _dbg_log("[MMU]Realloc [0x%x] to [0x%x]\n", old, ret);
    return ret;
}

public
void mmu_munmap(void* mem) {
#ifdef TARGET_HOST
    free(mem);
#else
    _free(mem);
#endif
}
