#include <assert.h>
#include <stdio.h>

char *heap_cur = (char*)5000;

void *kmalloc_align(unsigned int size, unsigned int alignment) {
    // Find the next address that matches alignment, return that number
    unsigned int next = (unsigned int)heap_cur;
    for (; ; next++) {
        if (next % alignment == 0) {
            break;
        }
    }

    heap_cur = next + size;
    return (void*)next;
}

void test_kmalloc_align() {
    void *p = kmalloc_align(4096, 4096);
    printf("%p\n", p);
    assert(p == (void*)8192);
    assert(heap_cur == (void*)(8192 + 4096));
}