#ifndef INCLUDE_SVGA_H
#define INCLUDE_SVGA_H

#define LFB_VADDR 0xE0000000

enum COLOR_PALLETE {COLOR_BLACK = 0, COLOR_WHITE};

void svga_draw_pixel(int x, int y, unsigned int color);
void svga_init();
#endif
