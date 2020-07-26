#include "drivers/svga.h"

#include "kheap.h"
#include "kinfo.h"
#include "paging.h"
#include "stdint.h"
#include "utils/debug.h"
#include "builddef.h"

static unsigned char* _svga_lfb = NULL;

private unsigned char* get_lfb_addr() {
    return _svga_lfb;
}

private void set_lfb_addr(unsigned char* fb) {
    _svga_lfb = fb;
}

private unsigned int svga_translate_rgb(unsigned char r, unsigned char g, unsigned char b) {
    unsigned int color = 0xffffff;

    struct kinfo *kinfo = get_kernel_info();
    struct multiboot_tag_framebuffer *tagfb = &kinfo->tagfb;
    switch (tagfb->common.framebuffer_type) {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
            unsigned best_distance, distance;
            struct multiboot_color *palette;

            palette = tagfb->framebuffer_palette;

            color = 0;
            best_distance = 4 * 256 * 256;

            for (unsigned short i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
                distance = (b - palette[i].blue) * (b - palette[i].blue) 
                        + (r - palette[i].red) * (r - palette[i].red) 
                        + (g - palette[i].green) * (g - palette[i].green);
                if (distance < best_distance) {
                    color = i;
                    best_distance = distance;
                }
            }
            break;
        }
        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB: {  // Max mask size 0x1f instead of 0xff
            unsigned char r_real = ((1 << tagfb->framebuffer_red_mask_size) - 1) * (r / 0xff);
            unsigned char g_real = ((1 << tagfb->framebuffer_green_mask_size) - 1) * (g / 0xff);
            unsigned char b_real = ((1 << tagfb->framebuffer_blue_mask_size) - 1) * (b / 0xff);
            color = (r_real << tagfb->framebuffer_red_field_position) | (g_real << tagfb->framebuffer_green_field_position) | (b_real << tagfb->framebuffer_blue_field_position);
            break;
        }
        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT: {
            color = '\\' | 0x0100;
            break;
        }
        default: {
            color = 0xffffffff;
            break;
        }
    }
    return color;
}

public void svga_draw_pixel(unsigned int x, unsigned int y, unsigned int color) {
    struct kinfo *kinfo = get_kernel_info();
    struct multiboot_tag_framebuffer *tagfb = &kinfo->tagfb;

    unsigned char* fb = get_lfb_addr();
    switch (tagfb->common.framebuffer_bpp) {
        case 8: {
            multiboot_uint8_t *pixel = fb + tagfb->common.framebuffer_pitch * y + x;
            *pixel = color;
            break;
        }
        case 15:
        case 16: {
            multiboot_uint16_t *pixel = fb + tagfb->common.framebuffer_pitch * y + 2 * x;
            *pixel = color;
            break;
        }
        case 24: {
            multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * y + 3 * x;
            *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
            break;
        }
        case 32: {
            multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * y + 4 * x;
            *pixel = color;
            break;
        }
    }
}

public void svga_draw_rect(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2, unsigned int color) {
    for (unsigned int y = y1; y < y2; ++y) {
        for (unsigned int x = x1; x < x2; ++x) {
            svga_draw_pixel(x, y, color);
        }
    }
}

void svga_init() {
    _dbg_log("Init svga\n");
    struct kinfo *kinfo = get_kernel_info();

    unsigned char *fb = kinfo->tagfb.common.framebuffer_addr;
    unsigned int *kpd = get_kernel_pd();
    unsigned int *page_tables = kmalloc_align(4096 * 256, 4096);
    paging_map(LFB_VADDR, (unsigned int)fb, kpd, page_tables);
    fb = (unsigned char *)LFB_VADDR;
    set_lfb_addr(fb);

    struct multiboot_tag_framebuffer *tagfb = &kinfo->tagfb;
    _dbg_log("[SVGA]fb type: [%u], bpp:[%u]\n", tagfb->common.framebuffer_type, tagfb->common.framebuffer_bpp);

    // Draw a white line
    unsigned int color = svga_translate_rgb(0xff, 0xff, 0xff);
    svga_draw_rect(10, 10, 300, 50, color);
}
