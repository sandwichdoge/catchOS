#include "kinfo.h"

#include "multiboot_info.h"
#include "builddef.h"
#include "utils/debug.h"
#include "utils/string.h"

// Kernel code section
extern size_t KERNEL_START_PHYS;
extern size_t KERNEL_START_VIRTUAL;
extern size_t KERNEL_END_PHYS;
extern size_t KERNEL_END_VIRTUAL;

static struct kinfo _kinfo;

struct kinfo *get_kernel_info() {
    return &_kinfo;
}

public
void kinfo_init() {
#ifndef TARGET_HOST
    _kinfo.kernel_start_phys = &KERNEL_START_PHYS;
    _kinfo.kernel_start_virtual = &KERNEL_START_VIRTUAL;
    _kinfo.kernel_end_phys = &KERNEL_END_PHYS;
    _kinfo.kernel_end_virtual = &KERNEL_END_VIRTUAL;
    _dbg_log("Kernel start:[0x%x]\n", _kinfo.kernel_start_virtual);
    _dbg_log("Kernel end:[0x%x]\n", _kinfo.kernel_end_virtual);

    // Take info account GRUB module loaded along with kernel. We shouldn't
    // overwrite it so we treat it as a part of kernel.
    /*
    if (mods) {
        if (mbinfo->mods_count) {
            struct multiboot_mod_list *mods = (struct multiboot_mod_list *)mbinfo->mods_addr;
            _kinfo.kernel_end_phys = (void *)(mods->mod_end + mods->pad);
            _kinfo.kernel_end_virtual = (void *)(mods->mod_end + mods->pad + 0x0);
        }
    }
    */
#endif
}
