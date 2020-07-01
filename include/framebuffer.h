#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#define FB_BLACK 0x0
#define FB_WHITE 0xf
#define SCR_W 80
#define SCR_H 24
#define SCR_SIZE SCR_W *SCR_H

void clr_screen(unsigned char bg);
int fb_get_scr_w();
int fb_get_scr_h();
void write_cell(unsigned int i, unsigned char c, unsigned char fg, unsigned char bg);
void write_chr(const char c, unsigned int *scrpos);
void write_str(const char *str, unsigned int *scrpos, unsigned int len);
void write_cstr(const char *str, unsigned int scrpos);
void move_cursor(unsigned int scrpos);
void scroll_down(unsigned int line_count);
#endif