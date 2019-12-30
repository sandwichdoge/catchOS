#ifndef INCLUDE_KHEAP_H
#define INCLUDE_KHEAP_H
void kheap_init();
void *kmalloc(unsigned int size);
void *kmalloc_align(unsigned int size, unsigned int alignment);
#endif