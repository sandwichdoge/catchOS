#include "drivers/svga.h"

#include "builddef.h"
#include "font.h"
#include "multiboot_info.h"
#include "mmu.h"
#include "paging.h"
#include "utils/bitmap.h"
#include "utils/debug.h"
#include "utils/maths.h"

static uint32_t SCR_W;
static uint32_t SCR_H;
static uint32_t SCR_COLUMNS;
static uint32_t SCR_ROWS;

static uint8_t *_svga_lfb = NULL;
static uint8_t *_backbuffer = NULL;

struct multiboot_tag_framebuffer *_tagfb;

private
uint8_t *get_lfb_addr() { return _svga_lfb; }

private
void set_lfb_addr(uint8_t *fb) { _svga_lfb = fb; }

static struct rgb_color black = {0x0, 0x0, 0x0};

public
void swap_backbuffer_to_front() {
    // Copy all data in backbuffer to front buffer to show on screen.
    uint8_t *fb = get_lfb_addr();
    uint32_t fb_size = SCR_H * _tagfb->common.framebuffer_pitch;
    _memcpy(fb, _backbuffer, fb_size);
}

public
uint32_t svga_translate_rgb(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color;
    switch (_tagfb->common.framebuffer_type) {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
            uint32_t best_distance, distance;
            struct multiboot_color *palette;

            palette = _tagfb->framebuffer_palette;

            color = 0;
            best_distance = 4 * 256 * 256;

            for (uint16_t i = 0; i < _tagfb->framebuffer_palette_num_colors; i++) {
                distance = (b - palette[i].blue) * (b - palette[i].blue) + (r - palette[i].red) * (r - palette[i].red) +
                           (g - palette[i].green) * (g - palette[i].green);
                if (distance < best_distance) {
                    color = i;
                    best_distance = distance;
                }
            }
            break;
        }
        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB: {  // Max mask size 0x1f instead of 0xff
            uint8_t r_real = ((1 << _tagfb->framebuffer_red_mask_size) - 1) & (uint8_t)(r * ((float)((1 << _tagfb->framebuffer_red_mask_size) - 1) / 0xff));
            uint8_t g_real = ((1 << _tagfb->framebuffer_green_mask_size) - 1) & (uint8_t)(g * ((float)((1 << _tagfb->framebuffer_green_mask_size) - 1) / 0xff));
            uint8_t b_real = ((1 << _tagfb->framebuffer_blue_mask_size) - 1) & (uint8_t)(b * ((float)((1 << _tagfb->framebuffer_blue_mask_size) - 1) / 0xff));

            color = (r_real << _tagfb->framebuffer_red_field_position) | (g_real << _tagfb->framebuffer_green_field_position) |
                    (b_real << _tagfb->framebuffer_blue_field_position);
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

public
uint32_t svga_get_scr_columns() { return SCR_COLUMNS; }

public
uint32_t svga_get_scr_rows() { return SCR_ROWS; }

public
void svga_move_cursor(uint32_t scrpos) {}

public
void svga_draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    uint8_t *fb = get_lfb_addr();
    switch (_tagfb->common.framebuffer_bpp) {
        case 8: {
            multiboot_uint8_t *pixel = (multiboot_uint8_t *)(fb + _tagfb->common.framebuffer_pitch * y + x);
            *pixel = color;
            pixel = _backbuffer + _tagfb->common.framebuffer_pitch * y + x;
            *pixel = color;
            break;
        }
        case 15:
        case 16: {
            multiboot_uint16_t *pixel = (multiboot_uint16_t *)(fb + _tagfb->common.framebuffer_pitch * y + 2 * x);
            *pixel = color;
            pixel = (multiboot_uint16_t *)(_backbuffer + _tagfb->common.framebuffer_pitch * y + 2 * x);
            *pixel = color;
            break;
        }
        case 24: {
            multiboot_uint32_t *pixel = (multiboot_uint32_t *)(fb + _tagfb->common.framebuffer_pitch * y + 3 * x);
            *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
            pixel = (multiboot_uint32_t *)(_backbuffer + _tagfb->common.framebuffer_pitch * y + 3 * x);
            *pixel = color;
            break;
        }
        case 32: {
            multiboot_uint32_t *pixel = (multiboot_uint32_t *)(fb + _tagfb->common.framebuffer_pitch * y + 4 * x);
            *pixel = color;
            pixel = (multiboot_uint32_t *)(_backbuffer + _tagfb->common.framebuffer_pitch * y + 4 * x);
            *pixel = color;
            break;
        }
    }
}

public
void svga_draw_rect(const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2, uint32_t color) {
    for (uint32_t y = y1; y < y2; ++y) {
        for (uint32_t x = x1; x < x2; ++x) {
            svga_draw_pixel(x, y, color);
        }
    }
}

// We use 7x9 text font. But actually draw 8x9 (1 empty right column as delimiter).
public
void svga_draw_char(const uint32_t x, const uint32_t y, uint8_t c, uint32_t color) {
    uint8_t *bitmap = font_get_char(c);
    for (uint32_t yy = 0; yy < FONT_H; ++yy) {
        for (uint32_t xx = 0; xx < FONT_W; ++xx) {
            int bit = bitmap_get_bit(bitmap, yy * FONT_W + xx);
            if (bit) {
                svga_draw_pixel(x + xx, y + yy, color);
            }
        }
    }
}

public
void svga_scroll_down(uint32_t lines) {
    uint8_t *fb = get_lfb_addr();
    if (lines > SCR_ROWS) {
        lines = SCR_ROWS;
    }

    _dbg_log("Lines to scroll: %u\n", lines);

    uint32_t line_size = _tagfb->common.framebuffer_pitch;
    uint32_t fb_size = SCR_H * line_size;
    uint32_t row_size = line_size * (FONT_H + 2);

    _memcpy(_backbuffer, _backbuffer + (lines * row_size), fb_size - line_size);
    _memset(_backbuffer + fb_size - row_size, 0, row_size);
    _memcpy(fb, _backbuffer, fb_size);
}

public
void svga_draw_char_cell(uint32_t *scrpos, uint8_t c, uint32_t color) {
    if (*scrpos + 1 > (SCR_ROWS * SCR_COLUMNS)) {
        svga_scroll_down(1);
        *scrpos -= SCR_COLUMNS;  // Go back 1 line.
    }
    uint32_t y = *scrpos / SCR_COLUMNS;
    uint32_t x = *scrpos % SCR_COLUMNS;

    svga_draw_char(FONT_W * x, (FONT_H + 2) * y, c, color);
    *scrpos = *scrpos + 1;
}

public
void svga_write_str(const char *str, uint32_t *scrpos, uint32_t len, uint32_t color) {
    // If next string overflows screen, scroll screen to make space for OF text
    uint32_t lines_to_scroll = 0;
    if (*scrpos + len > (SCR_ROWS * SCR_COLUMNS)) {
        if (len > SCR_ROWS * SCR_COLUMNS) len = SCR_ROWS * SCR_COLUMNS;

        lines_to_scroll = ceiling(*scrpos + len - (SCR_ROWS * SCR_COLUMNS), SCR_COLUMNS);
        uint32_t chars_to_scroll = lines_to_scroll * SCR_COLUMNS;
        svga_scroll_down(lines_to_scroll);
        *scrpos -= chars_to_scroll;  // Go back the same number of lines
        svga_draw_char_cell(scrpos, 'x', color);
    }

    for (uint32_t i = 0; i < len; i++) {
        svga_draw_char_cell(scrpos, str[i], color);
    }
}

public
void svga_clr_cell(uint32_t *scrpos) { svga_draw_char_cell(scrpos, 255, svga_translate_rgb(black.r, black.g, black.b)); }

public
void svga_clr_scr() {
    uint32_t color = svga_translate_rgb(black.r, black.g, black.b);
    svga_draw_rect(0, 0, SCR_W, SCR_H, color);
}

public
void svga_init() {
    static int32_t initialized = 0;
    if (initialized) {
        return;
    }

    _dbg_log("Init svga\n");
    struct multiboot_info *mbinfo = get_multiboot_info();

    uint8_t *fb = (uint8_t *)mbinfo->tagfb.common.framebuffer_addr;
    set_lfb_addr(fb);
    // Map address space for fb.
    uint32_t *kpd = get_kernel_pd();
    paging_map_table((uint32_t)fb, (uint32_t)fb, kpd);

    // Set global fb info so we won't have to get it again later.
    _tagfb = &mbinfo->tagfb;

    SCR_W = _tagfb->common.framebuffer_width;
    SCR_H = _tagfb->common.framebuffer_height;
    SCR_COLUMNS = SCR_W / FONT_W;
    SCR_ROWS = SCR_H / (FONT_H + 2);

    _backbuffer = mmu_mmap(SCR_H * _tagfb->common.framebuffer_pitch);  // Max possible lfb size for 640x480x32 vga (2560 = 32bit fb pitch).
    _memset(_backbuffer, 0, SCR_H * _tagfb->common.framebuffer_pitch);

    _dbg_log("[SVGA]fb type: [%u], bpp:[%u]\n", _tagfb->common.framebuffer_type, _tagfb->common.framebuffer_bpp);
    _dbg_log("[SVGA]width: [%u], height:[%u]\n", _tagfb->common.framebuffer_width, _tagfb->common.framebuffer_height);
    _dbg_screen("[SVGA]fb type: [%u], bpp:[%u]\n", _tagfb->common.framebuffer_type, _tagfb->common.framebuffer_bpp);

    initialized = 1;
}
