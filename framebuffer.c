#include "io.h"
#include "framebuffer.h"
#include "utils/string.h"

#define FB_COMMAND_PORT 0x3d4
#define FB_DATA_PORT 0x3d5
#define FB_HIGH_BYTE_CMD 14
#define FB_LOW_BYTE_CMD 15

volatile char *fb = (volatile char*)0x000B8000; // Video memory address

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

// Scroll screen down some lines
void scroll_down(unsigned int line_count) {
    static unsigned char buf[SCR_SIZE * 2];
    _memset(buf, '\0', sizeof(buf));

    _memcpy(fb + (line_count * SCR_W) * 2, buf, sizeof(buf));
    _memcpy(buf, fb, sizeof(buf));
}

// Write a string to framebuffer
void write_str(const char *str, unsigned int scrpos, unsigned int len) {
    // If next string overflows screen, scroll screen to make space for OF text
    if (scrpos + len >= SCR_SIZE) {
        unsigned int lines_to_scroll = (scrpos + len - SCR_SIZE) / SCR_W;

        if (lines_to_scroll > SCR_H) {
            lines_to_scroll = SCR_H;
        }

        scroll_down(lines_to_scroll);
        scrpos -= lines_to_scroll * SCR_W; // Go back the same number of lines
    }

    scrpos *= 2;
    unsigned int cur_pos = scrpos;

    for (unsigned int i = 0; i < len; i++) {
        write_cell(cur_pos, str[i], FB_BLACK, FB_WHITE);
        cur_pos += 2;
    }
}

// Write a string until NULL encountered
void write_cstr(const char *str, unsigned int scrpos) {
    scrpos *= 2;

    unsigned int cur_pos = scrpos;
    for (unsigned int i = 0; str[i]; i++) {
        write_cell(cur_pos, str[i], FB_BLACK, FB_WHITE);
        cur_pos += 2;
    }
}

void clr_screen(unsigned char bg) {
    for (unsigned int i = 0; i < SCR_W * SCR_H; i += 2) {
        write_cell(i, ' ', bg, bg);
    }
}
