#include "io.h"

#define FB_COMMAND_PORT 0x3d4
#define FB_DATA_PORT 0x3d5
#define FB_HIGH_BYTE_CMD 14
#define FB_LOW_BYTE_CMD 15

volatile char *fb = (volatile char*)0x000B8000;

// Write character to a cell in 80x24 framebuffer
void write_cell(unsigned int i, unsigned char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = ((fg & 0b00001111) << 4) | (bg & 0b00001111);
}

void move_cursor(int y, int x) {
    int pos = y * 80 + x;
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
    outb(FB_DATA_PORT, (pos >> 8) & 0x00ff); // High bytes
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
    outb(FB_DATA_PORT, pos & 0x00ff);        // Low bytes
}

