#include "io.h"
#include "framebuffer.h"

#define FB_COMMAND_PORT 0x3d4
#define FB_DATA_PORT 0x3d5
#define FB_HIGH_BYTE_CMD 14
#define FB_LOW_BYTE_CMD 15

volatile char *fb = (volatile char*)0x000B8000;

// Write character to a cell in 80x24 framebuffer
void write_cell(unsigned int scrpos, unsigned char c, unsigned char fg, unsigned char bg) {
    fb[scrpos] = c;
    fb[scrpos + 1] = ((fg & 0b00001111) << 4) | (bg & 0b00001111);
}

void move_cursor(int y, int x) {
    int pos = y * SCR_W + x;
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
    outb(FB_DATA_PORT, (pos >> 8) & 0x00ff); // High bytes
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
    outb(FB_DATA_PORT, pos & 0x00ff);        // Low bytes
}

// Write a string to framebuffer
void write_str(const char *str, unsigned int scrpos, unsigned int len) {
    if (scrpos % 2) {
        return;
    }

    unsigned int cur_pos = scrpos;

    for (unsigned int i = 0; i < len; i++) {
        write_cell(cur_pos, str[i], FB_BLACK, FB_WHITE);
        cur_pos += 2;
    }
}

void clr_screen(unsigned char bg) {
    for (unsigned int i = 0; i < SCR_W * SCR_H; i += 2) {
        write_cell(i, ' ', bg, bg);
    }
}