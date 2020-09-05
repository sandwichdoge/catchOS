#ifndef INCLUDE_SVGA_H
#define INCLUDE_SVGA_H

#include "stdint.h"

struct rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

uint32_t svga_get_scr_columns();
uint32_t svga_get_scr_rows();
void svga_move_cursor(uint32_t scrpos);
uint32_t svga_translate_rgb(uint8_t r, uint8_t g, uint8_t b);
void svga_clr_cell(uint32_t* scrpos);
void svga_clr_scr();
void svga_scroll_down(uint32_t lines);
void svga_draw_char_cell(uint32_t* scrpos, uint8_t c, uint32_t color);
void svga_write_str(const char* str, uint32_t* scrpos, uint32_t len, uint32_t color);
void svga_draw_rect(const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2, uint32_t color);
void svga_draw_pixel(const uint32_t x, const uint32_t y, uint32_t color);
void svga_flush_lfb();
void svga_init();

#endif
