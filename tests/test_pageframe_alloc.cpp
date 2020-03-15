extern "C" {
#include "pageframe_alloc.h"
#include "kheap.h"
}
#include "gtest/gtest.h"

extern unsigned char *_pageframe_bitmap;
extern unsigned int _pages_total_phys;
extern int _is_initialized;
unsigned char pf[1024] = {0};

static void init_pageframe() {
    _pageframe_bitmap = pf;
    _pages_total_phys = 4096;
    _pageframe_bitmap[0] = 0xff;
    _is_initialized = 1;
}

TEST(pageframe_alloc, basic) {
    init_pageframe();
    void *p = pageframe_alloc(7);
    printf("%p\n", p);
    EXPECT_NE(p, (void*)0);
}
