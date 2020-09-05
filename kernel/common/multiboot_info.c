#include "multiboot_info.h"
#include "builddef.h"
#include "utils/debug.h"
#include "utils/string.h"

static struct multiboot_info _mbinfo;

public
void multiboot_info_init(struct multiboot_tag* mb2) {
#ifndef TARGET_HOST
    size_t addr = (size_t)mb2;
    struct multiboot_tag* tag;
    for (tag = (struct multiboot_tag*)(addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((multiboot_uint8_t*)tag + ((tag->size + 7) & ~7))) {
        _dbg_log("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                _dbg_log("Command line = %s\n", ((struct multiboot_tag_string*)tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                _dbg_log("Boot loader name = %s\n", ((struct multiboot_tag_string*)tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                _dbg_log("Module at 0x%x-0x%x. Command line %s\n", ((struct multiboot_tag_module*)tag)->mod_start, ((struct multiboot_tag_module*)tag)->mod_end,
                         ((struct multiboot_tag_module*)tag)->cmdline);
                static int modcount = 0;
                _memcpy(&_mbinfo.mods[modcount++], (char*)tag, sizeof(struct multiboot_tag_module));
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                _dbg_log("mem_lower = %uKB, mem_upper = %uKB\n", ((struct multiboot_tag_basic_meminfo*)tag)->mem_lower,
                         ((struct multiboot_tag_basic_meminfo*)tag)->mem_upper);
                _mbinfo.phys_mem_lower = ((struct multiboot_tag_basic_meminfo*)tag)->mem_lower;
                _mbinfo.phys_mem_upper = ((struct multiboot_tag_basic_meminfo*)tag)->mem_upper;
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                _dbg_log("Boot device 0x%x,%u,%u\n", ((struct multiboot_tag_bootdev*)tag)->biosdev, ((struct multiboot_tag_bootdev*)tag)->slice,
                         ((struct multiboot_tag_bootdev*)tag)->part);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_memory_map_t* mmap;
                for (mmap = ((struct multiboot_tag_mmap*)tag)->entries; (multiboot_uint8_t*)mmap < (multiboot_uint8_t*)tag + tag->size;
                     mmap = (multiboot_memory_map_t*)((size_t)mmap + ((struct multiboot_tag_mmap*)tag)->entry_size))
                    _dbg_log(
                        " base_addr = 0x%x%x,"
                        " length = 0x%x%x, type = 0x%x\n",
                        (size_t)(mmap->addr >> 32), (size_t)(mmap->addr & 0xffffffff), (size_t)(mmap->len >> 32), (size_t)(mmap->len & 0xffffffff),
                        (size_t)mmap->type);
                break;
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*)tag;
                _dbg_log("LFB at[0x%x], w[%u], h[%u], bpp[%u]\n", tagfb->common.framebuffer_addr, tagfb->common.framebuffer_width,
                         tagfb->common.framebuffer_height, tagfb->common.framebuffer_bpp);
                _memcpy(&_mbinfo.tagfb, tagfb, sizeof(*tagfb));
                break;
            }
            case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
                struct multiboot_tag_old_acpi* acpi = (struct multiboot_tag_old_acpi*)tag;
                _mbinfo.rsdp = acpi->rsdp;
                _mbinfo.acpi_ver = 1;
                break;
            }
            case MULTIBOOT_TAG_TYPE_ACPI_NEW: {
                struct multiboot_tag_new_acpi* acpi = (struct multiboot_tag_new_acpi*)tag;
                _mbinfo.rsdp = acpi->rsdp;
                _mbinfo.acpi_ver = 2;
                break;
            }
        }
    }
#endif
}

struct multiboot_info* get_multiboot_info() {
    return &_mbinfo;
}