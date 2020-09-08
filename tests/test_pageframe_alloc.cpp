extern "C" {
#include "pageframe_alloc.h"
}
#include "gtest/gtest.h"

extern unsigned char* _pageframe_bitmap;
extern unsigned int _pages_total_phys;
extern int _is_initialized;
unsigned char pf[1024] = {0};

#define FIRST_USABLE_ADDR (void*)0x8000

static void init_pageframe() {
    _pageframe_bitmap = pf;
    _pages_total_phys = 4096;
    _pageframe_bitmap[0] = 0xff;
    _is_initialized = 1;
}

TEST(pageframe_alloc, basic) {
    init_pageframe();
    void* p = pageframe_alloc(7);
    printf("%p\n", p);
    EXPECT_NE(p, (void*)0);
    EXPECT_EQ(p, FIRST_USABLE_ADDR);
    pageframe_free(p, 7);
}

TEST(pageframe_alloc, reclaim) {
    init_pageframe();
    void* p = pageframe_alloc(7);
    printf("%p - ", p);
    EXPECT_NE(p, (void*)0);
    pageframe_free(p, 7);
    void* p2 = pageframe_alloc(3);
    printf("%p\n", p2);
    EXPECT_EQ(p2, p);
    pageframe_free(p2, 3);
}

TEST(pageframe_alloc, arbitrary_alloc) {
    init_pageframe();
    void* p = pageframe_alloc(7);
    void* p2 = pageframe_alloc(3);
    printf("%p - %p\n", p, p2);
    EXPECT_NE(p, p2);
    EXPECT_GT(p2, p);
    pageframe_free(p, 7);
    pageframe_free(p2, 3);
}

TEST(pageframe_alloc, arbitrary_alloc2) {
    init_pageframe();
    void* p = pageframe_alloc(4);
    void* p2 = pageframe_alloc(14);
    void* p3 = pageframe_alloc(1);
    void* p4 = pageframe_alloc(2);

    pageframe_free(p3, 1);
    pageframe_free(p, 4);
    pageframe_free(p2, 14);
    pageframe_free(p4, 2);

    void* final = pageframe_alloc(4);
    EXPECT_EQ(final, FIRST_USABLE_ADDR);
    pageframe_free(final, 4);
}

TEST(pageframe_alloc, bestfit_behaviour) {
    init_pageframe();
    void* p = pageframe_alloc(3);
    void* p2 = pageframe_alloc(14);
    void* p3 = pageframe_alloc(1);
    void* p4 = pageframe_alloc(2);

    EXPECT_EQ(p2, (char*)p + 0x3000);  // 3 pages
    printf("After 3 pages[%p]\n", p2);
    pageframe_free(p2, 14);

    void* final = pageframe_alloc(1);
    printf("Final[%p]\n", final);
    EXPECT_EQ(final, (void*)0xb000);

    pageframe_free(final, 1);
    pageframe_free(p3, 1);
    pageframe_free(p, 3);
    pageframe_free(p4, 2);
}

TEST(pageframe_alloc, bestfit_behaviour2) {
    init_pageframe();

    void* p = pageframe_alloc(4);
    void* p2 = pageframe_alloc(12);
    printf("%p\n", p2);

    pageframe_free(p, 4);
    pageframe_free(p2, 12);
}

TEST(pageframe_alloc, bestfit_behaviour3) {
    init_pageframe();
    void* p = pageframe_alloc(1);
    void* p2 = pageframe_alloc(1);
    printf("%p-%p\n", p, p2);
    pageframe_free(p, 1);
    pageframe_free(p2, 1);
}

TEST(pageframe_alloc, allocate_zero) {
    init_pageframe();
    void* p = pageframe_alloc(0);
    printf("%p\n", p);
    void* p2 = pageframe_alloc(0);
    printf("%p\n", p2);
}
