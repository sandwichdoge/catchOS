#include "io.h"
#include "framebuffer.h"
#include "utils/string.h"
#include "utils/debug.h"

#define FB_COMMAND_PORT 0x3d4
#define FB_DATA_PORT 0x3d5
#define FB_HIGH_BYTE_CMD 14
#define FB_LOW_BYTE_CMD 15

char *fb = (char*)0xC00B8000; // Video memory address

// Write character to a cell in 80x24 framebuffer
void write_cell(unsigned int scrpos, unsigned char c, unsigned char fg, unsigned char bg) {
    fb[scrpos] = c;
    fb[scrpos + 1] = ((fg & 0b00001111) << 4) | (bg & 0b00001111);
}

void move_cursor(unsigned int scrpos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
    outb(FB_DATA_PORT, (scrpos >> 8) & 0x00ff); // High bytes
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
    outb(FB_DATA_PORT, scrpos & 0x00ff);        // Low bytes
}

// Scroll screen down some lines
void scroll_down(unsigned int line_count) {
    if (line_count > SCR_H) {
        line_count = SCR_H;
    }

    static char buf[SCR_SIZE * 2];
    _memset(buf, '\0', sizeof(buf));

    _memcpy(fb + (line_count * SCR_W) * 2, buf, sizeof(buf));
    _memcpy(buf, fb, sizeof(buf));
}

// Framebuffer doesn't have the concept of linebreak, we have to implement it in the shell/stdin.
void write_chr(const char c, unsigned int *scrpos) {
    if (*scrpos + 1 > SCR_SIZE) {
        scroll_down(1);
        *scrpos -= SCR_W; // Go back 1 line
    }

    write_cell((*scrpos) * 2, c, FB_BLACK, FB_WHITE);
    *scrpos = *scrpos + 1;
}

// Write a string to framebuffer.
void write_str(const char *str, unsigned int *scrpos, unsigned int len) {
    // If next string overflows screen, scroll screen to make space for OF text
    unsigned int lines_to_scroll = 0;
    if (*scrpos + len > SCR_SIZE) {
        lines_to_scroll = ((*scrpos + len - SCR_SIZE) / SCR_W) + 1; // Divide rounded down, so we add 1

        scroll_down(lines_to_scroll);
        *scrpos -= lines_to_scroll * SCR_W; // Go back the same number of lines
        write_cell(*scrpos * 2, 'x', FB_BLACK, FB_WHITE);
    }

    unsigned int cur_pos = (*scrpos) * 2;

    for (unsigned int i = 0; i < len; i++) {
        write_cell(cur_pos, str[i], FB_BLACK, FB_WHITE);
        cur_pos += 2;
    }

    *scrpos = *scrpos + len;
}

// Write a string until NULL encountered
void write_cstr(const char *str, unsigned int scrpos) {
    // TODO Handle scrolling
    scrpos *= 2;

    unsigned int cur_pos = scrpos;
    for (unsigned int i = 0; str[i]; i++) {
        write_cell(cur_pos, str[i], FB_BLACK, FB_WHITE);
        cur_pos += 2;
    }
}

int fb_get_scr_w() {
    return SCR_W;
}

int fb_get_scr_h() {
    return SCR_H;
}

void clr_screen(unsigned char bg) {
    for (unsigned int i = 0; i < SCR_W * SCR_H; i += 2) {
        write_cell(i, '\0', bg, bg);
    }
}
