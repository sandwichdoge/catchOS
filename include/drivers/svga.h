#ifndef INCLUDE_SVGA_H
#define INCLUDE_SVGA_H

struct rgb_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

void svga_draw_rect(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2, unsigned int color);
void svga_draw_pixel(const unsigned int x, const unsigned int y, unsigned int color);
void svga_init();

#endif
