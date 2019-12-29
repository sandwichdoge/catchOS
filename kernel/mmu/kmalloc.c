#include "kmalloc.h"
#include "kinfo.h"
// Kernel stack pointer is at 0x1fffff
// Kernel program is at 0x200000 in memory
// A quick way is to start the heap at kernel program's end

extern unsigned char KERNEL_END_PHYS; // Defined in link.ld
char* heap_start = &KERNEL_END_PHYS; // TODO take into account additional GRUB modules

// Allocate a contiguous chunk in physical memory
void *kmalloc(unsigned int size) {
    heap_start += size;
    return (void*)heap_start;
}

void kfree(void* ptr) {
    
}