#include "pageframe_alloc.h"
#include "paging.h"
#include "builddef.h"
#include "utils/debug.h"


public void mmu_init() {
    paging_init();
    pageframe_alloc_init();
}
