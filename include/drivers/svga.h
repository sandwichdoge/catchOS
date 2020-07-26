#ifndef INCLUDE_SVGA_H
#define INCLUDE_SVGA_H

#define LFB_VADDR 0xE0000000

void svga_draw_rect(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2, unsigned int color);
void svga_draw_pixel(const unsigned int x, const unsigned int y, unsigned int color);
void svga_init();

#endif
