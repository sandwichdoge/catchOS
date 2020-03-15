extern "C" {
#include "pageframe_alloc.h"
#include "kheap.h"
}
#include "gtest/gtest.h"

extern unsigned char *_pageframe_bitmap;
extern int _is_initialized;
unsigned char pf[1024] = {0};

static void init_pageframe() {
    _pageframe_bitmap = pf;
    _pageframe_bitmap[0] = 0xff;
    _is_initialized = 1;
}

TEST(pageframe_alloc, basic) {
    init_pageframe();
    void *p = pageframe_alloc(7);
    EXPECT_NE(p, (void*)0);
}
