#include "drivers/svga.h"

#include "kheap.h"
#include "kinfo.h"
#include "paging.h"
#include "stdint.h"
#include "timer.h"
#include "utils/debug.h"

void svga_init() {
    _dbg_log("Init svga\n");
    struct kinfo *kinfo = get_kernel_info();

    unsigned char *fb = kinfo->tagfb.common.framebuffer_addr;
    _dbg_log("[SVGA]lfb addr: 0x%x\n", fb);

    unsigned int *kpd = get_kernel_pd();
    unsigned int *page_tables = kmalloc_align(4096 * 256, 4096);
    paging_map(LFB_VADDR, (unsigned int)fb, kpd, page_tables);
    fb = (unsigned char *)LFB_VADDR;

    multiboot_uint32_t color;
    unsigned i;
    struct multiboot_tag_framebuffer *tagfb = &kinfo->tagfb;

    switch (tagfb->common.framebuffer_type) {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
            unsigned best_distance, distance;
            struct multiboot_color *palette;

            palette = tagfb->framebuffer_palette;

            color = 0;
            best_distance = 4 * 256 * 256;

            for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
                distance = (0xff - palette[i].blue) * (0xff - palette[i].blue) + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
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
                *pixel = color;
            } break;
        }
    }
}
