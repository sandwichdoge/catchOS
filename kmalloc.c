#include "kmalloc.h"

// Kernel stack pointer is at 0x10000
// Kernel program is at 0x100000 in memory
// A quick way is to start the heap at kernel program's end

extern unsigned char KERNEL_PROGRAM_END; // Defined in link.ld
char* heap_start = &KERNEL_PROGRAM_END;

void *kmalloc(unsigned int size) {
    heap_start += size;
    return (void*)heap_start;
}

void free(void* ptr) {
    
}