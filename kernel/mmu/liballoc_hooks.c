#include "liballoc.h"
#include "pageframe_alloc.h"

int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}

void* liballoc_alloc(int pages) {
    return pageframe_alloc((unsigned int)pages);
}

int liballoc_free(void* p, int pages) {
    pageframe_free(p, (unsigned int)pages);
    return 0;
}
