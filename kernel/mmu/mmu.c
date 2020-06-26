#include "pageframe_alloc.h"
#include "paging.h"
#include "builddef.h"
#include "mmu.h"
#include "utils/debug.h"

private void test_pageframe_firstpage() {
    void *p = pageframe_alloc(1);
    _dbg_log("Userland heap at: %x\n", p);
    pageframe_free(p, 1);
}

public void mmu_init() {
    paging_init();
    pageframe_alloc_init();
    test_pageframe_firstpage();

}

public void* mmu_sbrk(unsigned int size) {
    _dbg_log("sbrk [%u]\n", size);
    return NULL;
}

public void mmu_free(void* mem) {

}