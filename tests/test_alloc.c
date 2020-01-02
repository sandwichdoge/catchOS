#include <assert.h>
#include <stdio.h>

char *heap_cur = (char*)5000;
char map[4096] = {0};

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
    assert(p == (void*)8192);
    assert(heap_cur == (void*)(8192 + 4096));
}

void set_bit_from_page(char *bitmap, unsigned int page_no) {
    // Set <page_no>th bit in <bitmap>
    unsigned int byte_no = page_no / 8;
    unsigned int carry_bit = byte_no % 8;
    bitmap[byte_no] = (1 << carry_bit);
}

void clear_bit_from_page(char *bitmap, unsigned int page_no) {
    unsigned int byte_no = page_no / 8;
    unsigned int carry_bit = byte_no % 8;
    bitmap[byte_no] &= ~(1 << carry_bit);
}

char get_bit_from_page(char *bitmap, unsigned int page_no) {
    unsigned int byte_no = page_no / 8;
    unsigned int carry_bit = byte_no % 8;
    return ((bitmap[byte_no] & (1 << carry_bit)) >> carry_bit);
}

void test_page_to_bit() {
    char ret2 = get_bit_from_page(map, 10);
    assert(ret2 == 0);
    printf("[%d]\n", ret2);


    set_bit_from_page(map, 9);
    char ret1 = get_bit_from_page(map, 9);
    printf("[%d]\n", ret1);
    assert(ret1 == 1);

    set_bit_from_page(map, 679);
    char ret3 = get_bit_from_page(map, 679);
    printf("[%d]\n", ret3);
    assert(ret3 == 1);

    set_bit_from_page(map, 678);
    char ret4 = get_bit_from_page(map, 678);
    printf("[%d]\n", ret4);
    assert(ret4 == 1);

    clear_bit_from_page(map, 678);
    char ret5 = get_bit_from_page(map, 678);
    printf("[%d]\n", ret5);
    assert(ret5 == 0);
}