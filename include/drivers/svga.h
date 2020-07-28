#ifndef INCLUDE_SVGA_H
#define INCLUDE_SVGA_H

struct rgb_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

int svga_get_scr_columns();
int svga_get_scr_rows();
void svga_move_cursor(unsigned int scrpos);
unsigned int svga_translate_rgb(unsigned char r, unsigned char g, unsigned char b);
void svga_clr_cell(unsigned int *scrpos);
void svga_clr_scr();
void svga_scroll_down(unsigned int lines);
void svga_draw_char_cell(unsigned int *scrpos, unsigned char c, unsigned int color);
void svga_write_str(const char *str, unsigned int *scrpos, unsigned int len, unsigned int color);
void svga_draw_rect(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2, unsigned int color);
void svga_draw_pixel(const unsigned int x, const unsigned int y, unsigned int color);
void svga_init();

#endif
