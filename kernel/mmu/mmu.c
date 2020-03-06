#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"

void mmu_init() {
    paging_init();
    pageframe_alloc_init();
}
