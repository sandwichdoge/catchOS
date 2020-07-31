#include "kinfo.h"

#include "builddef.h"
#include "utils/debug.h"

// Kernel code section
extern unsigned int KERNEL_START_PHYS;
extern unsigned int KERNEL_START_VIRTUAL;
extern unsigned int KERNEL_END_PHYS;
extern unsigned int KERNEL_END_VIRTUAL;

static struct kinfo _kinfo;

struct kinfo *get_kernel_info() {
    return &_kinfo;
}

public
void kinfo_init(struct multiboot_tag *mb2) {
#ifndef TARGET_HOST
    unsigned int addr = (unsigned int)mb2;
    struct multiboot_tag *tag;
    for (tag = (struct multiboot_tag *)(addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        _dbg_log("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                _dbg_log("Command line = %s\n", ((struct multiboot_tag_string *)tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                _dbg_log("Boot loader name = %s\n", ((struct multiboot_tag_string *)tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                _dbg_log("Module at 0x%x-0x%x. Command line %s\n", ((struct multiboot_tag_module *)tag)->mod_start,
                         ((struct multiboot_tag_module *)tag)->mod_end, ((struct multiboot_tag_module *)tag)->cmdline);
                _memcpy(&_kinfo.mod, (struct multiboot_tag_module *)tag, sizeof(_kinfo));
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                _dbg_log("mem_lower = %uKB, mem_upper = %uKB\n", ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
                         ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
                _kinfo.phys_mem_lower = ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower;
                _kinfo.phys_mem_upper = ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper;
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                _dbg_log("Boot device 0x%x,%u,%u\n", ((struct multiboot_tag_bootdev *)tag)->biosdev, ((struct multiboot_tag_bootdev *)tag)->slice,
                         ((struct multiboot_tag_bootdev *)tag)->part);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_memory_map_t *mmap;
                for (mmap = ((struct multiboot_tag_mmap *)tag)->entries; (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
                    _dbg_log(
                        " base_addr = 0x%x%x,"
                        " length = 0x%x%x, type = 0x%x\n",
                        (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff),
                        (unsigned)mmap->type);
                break;
            } 
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
                _dbg_log("LFB at[0x%x], w[%u], h[%u], bpp[%u]", tagfb->common.framebuffer_addr, tagfb->common.framebuffer_width,
                         tagfb->common.framebuffer_height, tagfb->common.framebuffer_bpp);
                _memcpy((char *)&_kinfo.tagfb, (char *)tagfb, sizeof(*tagfb));
                break;
            }
            case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
                struct multiboot_tag_old_acpi *acpi = (struct multiboot_tag_old_acpi *)tag;
                _kinfo.rsdp = acpi->rsdp;
                _kinfo.acpi_ver = 1;
                break;
            }
            case MULTIBOOT_TAG_TYPE_ACPI_NEW: {
                struct multiboot_tag_new_acpi *acpi = (struct multiboot_tag_new_acpi *)tag;
                _kinfo.rsdp = acpi->rsdp;
                _kinfo.acpi_ver = 2;
                break;
            }
        }
    }

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

public
void kinfo_set_phys_mem_lower(unsigned int phys_mem_lower) { _kinfo.phys_mem_lower = phys_mem_lower; }

public
void kinfo_set_phys_mem_upper(unsigned int phys_mem_upper) { _kinfo.phys_mem_upper = phys_mem_upper; }
