#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"

void mmu_init() {
    paging_init();
    pageframe_alloc_init();

    void* p;

    p = pageframe_alloc(7);
    pageframe_free(p, 7);
    _dbg_set_edi((unsigned int)p);
    _dbg_break();

    p = pageframe_alloc(20);
    //_dbg_set_edi((unsigned int)p);
    //_dbg_break();
}
