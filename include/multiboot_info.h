#ifndef INCLUDE_MULTIBOOT_INFO_H
#define INCLUDE_MULTIBOOT_INFO_H
#include "multiboot.h"
#include "stddef.h"
#include "stdint.h"

struct multiboot_info {
    size_t phys_mem_lower;
    size_t phys_mem_upper;
    struct multiboot_tag_framebuffer tagfb;
    struct multiboot_tag_module mods[2];
    void *rsdp;
    int32_t acpi_ver;
};

void multiboot_info_init(struct multiboot_tag *mb2);
struct multiboot_info *get_multiboot_info();

#endif
