#include "kheap.h"
#include "kinfo.h"
// Kernel stack pointer is at 0x1fffff
// Kernel program is at 0x200000 in phys memory
// A quick way is to start the heap at kernel program's end

char* heap_cur = (char*)0;

void kheap_init() {
    struct kinfo *kinfo = get_kernel_info();
    heap_cur = kinfo->kernel_end_virtual;
}

// Allocate a contiguous chunk in physical memory
void *kmalloc(unsigned int size) {
    void *ret = heap_cur;
    heap_cur += size;
    return ret;
}

void *kmalloc_align(unsigned int size, unsigned int alignment) {
    // Find the next address that matches alignment, return that number
    unsigned int next = (unsigned int)heap_cur;
    for (; ; next++) {
        if (next % alignment == 0) {
            break;
        }
    }

    heap_cur = (char*)(next + size);
    return (void*)next;
}

void kfree(void* ptr) {
    
}