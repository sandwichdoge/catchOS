#ifndef INCLUDE_SYSCALL_H
#define INCLUDE_SYSCALL_H

// Global keyboard press handler. Userland will register with this.
void (*_kb_handler_cb)(unsigned char c);

void syscall_init();

// Subscribe to keyboard events.
void syscall_register_kb_handler(void (*kb_handler)(unsigned char c));

// Read stdin buffer, this buffer is emptied after each read.
void syscall_read_cin(char *out, unsigned int len);

// Print a string to screen, return characters lost to auto-scrolling.
unsigned int syscall_fb_write_str(const char *str, unsigned int scrpos, unsigned int len);

// Clear video screen.
void syscall_fb_clr_scr(void);

// Move the blinking cursor.
void syscall_fb_mov_cursr(unsigned int scrpos);

// Write data to serial port.
void syscall_serial_writestr(char *str, unsigned int len);

#endif
