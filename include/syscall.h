#ifndef INCLUDE_SYSCALL_H
#define INCLUDE_SYSCALL_H
#include "interrupt.h"  // struct cpu_state

enum SYSCALL_NUM { SYSCALL_MMAP = 2, SYSCALL_MUNMAP = 3 };

// Global keyboard press handler. Userland will register with this.
void (*_kb_handler_cb)(unsigned char c);

void syscall_init();

// Get VGA size
int syscall_fb_get_scr_w();
int syscall_fb_get_scr_h();

void syscall_fb_scroll_down(unsigned int lines);

// Subscribe to keyboard events.
void syscall_register_kb_handler(void (*kb_handler)(unsigned char c));

// Print a char to screen.
void syscall_fb_write_chr(const char c, unsigned int *scrpos);

// Print a string to screen.
void syscall_fb_write_str(const char *str, unsigned int *scrpos, unsigned int len);

// Draw a rectangle on screen.
void syscall_fb_draw_rect(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2);

// Set global brush's color.
void syscall_fb_brush_set_color(unsigned char r, unsigned char g, unsigned char b);

// Clear a cell.
void syscall_fb_clr_cell(unsigned int *scrpos);

// Clear video screen.
void syscall_fb_clr_scr(void);

// Move the blinking cursor.
void syscall_fb_mov_cursr(unsigned int scrpos);

// Write data to serial port.
void syscall_serial_writestr(char *str, unsigned int len);

#endif
