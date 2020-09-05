#ifndef INCLUDE_KINFO_H
#define INCLUDE_KINFO_H
#include "stddef.h"
#include "stdint.h"

struct kinfo {
    void* kernel_start_phys;
    void* kernel_start_virtual;
    void* kernel_end_phys;
    void* kernel_end_virtual;
    /*size_t phys_mem_lower;
    size_t phys_mem_upper;
    struct multiboot_tag_framebuffer tagfb;
    struct multiboot_tag_module mods[2];
    void *rsdp;
    int32_t acpi_ver;*/
    int32_t is_paging_enabled;
};
struct kinfo* get_kernel_info();
// Get kernel and hardware info
void kinfo_init();
#endif
