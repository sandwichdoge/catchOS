#ifndef INCLUDE_SYSCALL_H
#define INCLUDE_SYSCALL_H
#include "interrupt.h"  // struct cpu_state
#include "stddef.h"
#include "stdint.h"

enum SYSCALL_NUM { SYSCALL_MMAP = 2, SYSCALL_MUNMAP = 3 };

// Global keyboard press handler. Userland will register with this.
void (*_kb_handler_cb)(unsigned char c);

void syscall_init();

// Get VGA size
uint32_t syscall_fb_get_scr_cols();
uint32_t syscall_fb_get_scr_rows();

void syscall_fb_scroll_down(size_t lines);

// Subscribe to keyboard events.
void syscall_register_kb_handler(void (*kb_handler)(unsigned char c));

// Print a char to screen.
void syscall_fb_write_chr(const char c, size_t* scrpos);

// Print a string to screen.
void syscall_fb_write_str(const char* str, size_t* scrpos, size_t len);

// Draw a rectangle on screen.
void syscall_fb_draw_rect(const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2);

// Set global brush's color.
void syscall_fb_brush_set_color(unsigned char r, unsigned char g, unsigned char b);

// Clear a cell.
void syscall_fb_clr_cell(size_t* scrpos);

// Clear video screen.
void syscall_fb_clr_scr(void);

// Move the blinking cursor.
void syscall_fb_mov_cursr(size_t scrpos);

// Write data to serial port.
void syscall_serial_writestr(char* str, size_t len);

#endif
