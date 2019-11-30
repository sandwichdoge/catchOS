#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#define FB_BLACK 0x0
#define FB_WHITE 0xf

void write_cell(unsigned int i, unsigned char c, unsigned char fg, unsigned char bg);
void move_cursor(int y, int x);
#endif