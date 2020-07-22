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
    unsigned int mem_lower = 0;
    unsigned int mem_upper = 0;

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
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                _dbg_log("mem_lower = %uKB, mem_upper = %uKB\n", ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
                         ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
                mem_lower = ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower;
                mem_upper = ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper;
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                _dbg_log("Boot device 0x%x,%u,%u\n", ((struct multiboot_tag_bootdev *)tag)->biosdev, ((struct multiboot_tag_bootdev *)tag)->slice,
                         ((struct multiboot_tag_bootdev *)tag)->part);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_memory_map_t *mmap;

                _dbg_log("mmap\n");

                for (mmap = ((struct multiboot_tag_mmap *)tag)->entries; (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
                    _dbg_log(
                        " base_addr = 0x%x%x,"
                        " length = 0x%x%x, type = 0x%x\n",
                        (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff),
                        (unsigned)mmap->type);
            } break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                multiboot_uint32_t color;
                unsigned i;
                struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
                void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;

                switch (tagfb->common.framebuffer_type) {
                    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
                        unsigned best_distance, distance;
                        struct multiboot_color *palette;

                        palette = tagfb->framebuffer_palette;

                        color = 0;
                        best_distance = 4 * 256 * 256;

                        for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
                            distance =
                                (0xff - palette[i].blue) * (0xff - palette[i].blue) + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
                            if (distance < best_distance) {
                                color = i;
                                best_distance = distance;
                            }
                        }
                    } break;

                    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                        color = ((1 << tagfb->framebuffer_blue_mask_size) - 1) << tagfb->framebuffer_blue_field_position;
                        break;

                    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                        color = '\\' | 0x0100;
                        break;

                    default:
                        color = 0xffffffff;
                        break;
                }

                for (i = 0; i < tagfb->common.framebuffer_width && i < tagfb->common.framebuffer_height; i++) {
                    switch (tagfb->common.framebuffer_bpp) {
                        case 8: {
                            multiboot_uint8_t *pixel = fb + tagfb->common.framebuffer_pitch * i + i;
                            *pixel = color;
                        } break;
                        case 15:
                        case 16: {
                            multiboot_uint16_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 2 * i;
                            *pixel = color;
                        } break;
                        case 24: {
                            multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 3 * i;
                            *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
                        } break;

                        case 32: {
                            multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
                            _dbg_log("PIXEL:0x%x, color:0x%x\n", pixel, color);
                            //*pixel = color;
                        } break;
                    }
                }
                break;
            }
        }
    }

    if (mb2) {
        _kinfo.phys_mem_lower = mem_lower;
        _kinfo.phys_mem_upper = mem_upper;
    } else {  // TODO: Ask BIOS for mem limits, hardcode for 32MB for now.
        _kinfo.phys_mem_lower = 0x27c;
        _kinfo.phys_mem_upper = 0x7bc0;
    }

    _kinfo.kernel_start_phys = &KERNEL_START_PHYS;
    _kinfo.kernel_start_virtual = &KERNEL_START_VIRTUAL;
    _kinfo.kernel_end_phys = &KERNEL_END_PHYS;
    _kinfo.kernel_end_virtual = &KERNEL_END_VIRTUAL;

    _dbg_break();

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
