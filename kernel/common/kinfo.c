#include "kinfo.h"
#include "utils/debug.h"

// Kernel code section
extern unsigned int KERNEL_START_PHYS;
extern unsigned int KERNEL_START_VIRTUAL;
extern unsigned int KERNEL_END_PHYS;
extern unsigned int KERNEL_END_VIRTUAL;

static struct kinfo _kinfo;

struct kinfo* get_kernel_info() {
    return &_kinfo;
}

void kinfo_init(multiboot_info_t *mbinfo) {
    if (mbinfo) {
        _kinfo.phys_mem_lower = mbinfo->mem_lower;
        _kinfo.phys_mem_upper = mbinfo->mem_upper;
    } else { // TODO: Ask BIOS for mem limits, hardcode for 32MB for now.
        _kinfo.phys_mem_lower = 0x27c;
        _kinfo.phys_mem_upper = 0x7bc0;
    }

    _kinfo.kernel_start_phys = &KERNEL_START_PHYS;
    _kinfo.kernel_start_virtual = &KERNEL_START_VIRTUAL;
    _kinfo.kernel_end_phys = &KERNEL_END_PHYS;
    _kinfo.kernel_end_virtual = &KERNEL_END_VIRTUAL;

    // Take info account GRUB module loaded along with kernel. We shouldn't 
    // overwrite it so we treat it as a part of kernel.
    if (mbinfo) {
        if (mbinfo->mods_count) {
            struct multiboot_mod_list *mods = (struct multiboot_mod_list *)mbinfo->mods_addr;
            _kinfo.kernel_end_phys += (mods->mod_start - mods->mod_end);
            _kinfo.kernel_end_virtual += (mods->mod_start - mods->mod_end);
        }
    }
}

void kinfo_set_phys_mem_lower(unsigned int phys_mem_lower) {
    _kinfo.phys_mem_lower = phys_mem_lower;
}

void kinfo_set_phys_mem_upper(unsigned int phys_mem_upper) {
    _kinfo.phys_mem_upper = phys_mem_upper;
}
