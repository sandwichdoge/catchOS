#ifndef INCLUDE_KINFO_H
#define INCLUDE_KINFO_H
#include "multiboot.h"

struct kinfo {
    void *kernel_start_phys;
    void *kernel_start_virtual;
    void *kernel_end_phys;
    void *kernel_end_virtual;
    unsigned int phys_mem_lower;
    unsigned int phys_mem_upper;
};
struct kinfo *get_kernel_info();
// Get kernel and hardware info
void kinfo_init(multiboot_info_t *mbinfo);
void kinfo_set_phys_mem_lower(unsigned int phys_mem_lower);
void kinfo_set_phys_mem_upper(unsigned int phys_mem_upper);
#endif
