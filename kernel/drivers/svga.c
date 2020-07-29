#include "drivers/svga.h"

#include "kheap.h"
#include "kinfo.h"
#include "paging.h"
#include "stdint.h"
#include "utils/bitmap.h"
#include "utils/debug.h"
#include "font.h"
#include "mmu.h"
#include "builddef.h"

#define SCR_W 640
#define SCR_H 480
#define SCR_COLUMNS (SCR_W / FONT_W)
#define SCR_ROWS (SCR_H / (FONT_H + 2))

static unsigned char* _svga_lfb = NULL;

private unsigned char* get_lfb_addr() {
    return _svga_lfb;
}

private void set_lfb_addr(unsigned char* fb) {
    _svga_lfb = fb;
}

static struct rgb_color black = {0x0, 0x0, 0x0};
static struct rgb_color yellow = {0xff, 0xff, 0x0};

public unsigned int svga_translate_rgb(unsigned char r, unsigned char g, unsigned char b) {
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

public int svga_get_scr_columns() {
    return SCR_COLUMNS;
}

public int svga_get_scr_rows() {
    return SCR_ROWS;
}

public void svga_move_cursor(unsigned int scrpos) {
    
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

// We use 7x9 text font. But actually draw 8x9 (1 empty right column as delimiter).
public void svga_draw_char(const unsigned int x, const unsigned int y, unsigned char c, unsigned int color) {
    unsigned char *bitmap = font_get_char(c);
    for (unsigned int yy = 0; yy < FONT_H; ++yy) {
        for (unsigned int xx = 0; xx < FONT_W; ++xx) {
            int bit = bitmap_get_bit(bitmap, yy * FONT_W + xx);
            if (bit) {
                svga_draw_pixel(x + xx, y + yy, color);
            }
        }
    }
}

public void svga_scroll_down(unsigned int lines) {
    struct kinfo *kinfo = get_kernel_info();
    struct multiboot_tag_framebuffer *tagfb = &kinfo->tagfb;
    unsigned char* fb = get_lfb_addr();
    if (lines > SCR_ROWS) {
        lines = SCR_ROWS;
    }

    _dbg_log("Lines to scroll: %u\n", lines);

    static char buf[2560 * SCR_H]; // Max possible lfb size for 640x480x32 vga (2560 = 32bit fb pitch).
    _memset(buf, 0, sizeof(buf));

    unsigned int fb_size = SCR_H * tagfb->common.framebuffer_pitch;
    unsigned int row_size = tagfb->common.framebuffer_pitch * (FONT_H + 2);

    _memcpy(buf, fb + (lines * row_size), fb_size);
    _memcpy(fb, buf, fb_size);
}

public void svga_draw_char_cell(unsigned int *scrpos, unsigned char c, unsigned int color) {
    if (*scrpos + 1 > (SCR_ROWS * SCR_COLUMNS)) {
        svga_scroll_down(1);
        *scrpos -= SCR_COLUMNS; // Go back 1 line.
    }
    unsigned int y = *scrpos / SCR_COLUMNS;
    unsigned int x = *scrpos % SCR_COLUMNS;

    svga_draw_char(FONT_W * x, (FONT_H + 2) * y, c, color);
    *scrpos = *scrpos + 1;
}

private unsigned int ceiling(unsigned int dividend, unsigned int divisor) {
    return (dividend + (divisor / 2)) / divisor;
}

public void svga_write_str(const char *str, unsigned int *scrpos, unsigned int len, unsigned int color) {
    // If next string overflows screen, scroll screen to make space for OF text
    unsigned int lines_to_scroll = 0;
    if (*scrpos + len > (SCR_ROWS * SCR_COLUMNS)) {
        if (len > SCR_ROWS * SCR_COLUMNS) len = SCR_ROWS * SCR_COLUMNS;

        lines_to_scroll = ceiling(*scrpos + len - (SCR_ROWS * SCR_COLUMNS), SCR_COLUMNS);
        unsigned int chars_to_scroll = lines_to_scroll * SCR_COLUMNS;
        svga_scroll_down(lines_to_scroll);
        *scrpos -= chars_to_scroll;  // Go back the same number of lines
        svga_draw_char_cell(scrpos, 'x', color);
    }

    for (unsigned int i = 0; i < len; i++) {
        svga_draw_char_cell(scrpos, str[i], color);
    }
}

public void svga_clr_cell(unsigned int *scrpos) {
    svga_draw_char_cell(scrpos, 255, svga_translate_rgb(black.r, black.g, black.b));
}

public void svga_clr_scr() {
    unsigned int color = svga_translate_rgb(black.r, black.g, black.b);
    svga_draw_rect(0, 0, SCR_W, SCR_H, color);
}

public void svga_init() {
    _dbg_log("Init svga\n");
    struct kinfo *kinfo = get_kernel_info();

    unsigned char *fb = kinfo->tagfb.common.framebuffer_addr;
    unsigned int *kpd = get_kernel_pd();
    paging_map_table((unsigned int)fb, (unsigned int)fb, kpd);
    set_lfb_addr(fb);

    struct multiboot_tag_framebuffer *tagfb = &kinfo->tagfb;
    _dbg_log("[SVGA]fb type: [%u], bpp:[%u]\n", tagfb->common.framebuffer_type, tagfb->common.framebuffer_bpp);
    _dbg_screen("[SVGA]fb type: [%u], bpp:[%u], address:[0x%x]\n", tagfb->common.framebuffer_type, tagfb->common.framebuffer_bpp, fb);
}
