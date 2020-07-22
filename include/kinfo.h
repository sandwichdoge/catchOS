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
    void *lfb_addr; // Linear Framebuffer - Map it to 0xE0000000
    unsigned char lfb_type; // MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED
    unsigned int lfb_bpp; // 8,16,24,32
    unsigned int lfb_width;
    unsigned int lfb_height;
    unsigned int lfb_pitch;
};
struct kinfo *get_kernel_info();
// Get kernel and hardware info
void kinfo_init(struct multiboot_tag *mb);
void kinfo_set_phys_mem_lower(unsigned int phys_mem_lower);
void kinfo_set_phys_mem_upper(unsigned int phys_mem_upper);
#endif
