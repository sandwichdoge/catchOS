#include "kheap.h"
#include "kinfo.h"
// Kernel stack pointer is at 0x1fffff
// Kernel program is at 0x200000 in phys memory
// A quick way is to start the heap at kernel program's end

char* heap_start = (char*)0;

void kheap_init() {
    struct kinfo *kinfo = get_kernel_info();
    heap_start = kinfo->kernel_end_virtual;
}

// Allocate a contiguous chunk in physical memory
void *kmalloc(unsigned int size) {
    void *ret = heap_start;
    heap_start += size;
    return ret;
}

void kfree(void* ptr) {
    
}