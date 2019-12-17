#ifndef INCLUDE_SYSCALL_H
#define INCLUDE_SYSCALL_H
// Global keyboard press handler. Userland will register with this.
void (*_kb_handler_cb)(unsigned char c);

// Subscribe to keyboard events.
void syscall_register_kb_handler(void (*kb_handler)(unsigned char c));

// Print a string to screen.
void syscall_fb_write_str(const char *str, unsigned int scrpos, unsigned int len);

// Clear video screen.
void syscall_fb_clr_scr(void);

// Write data to serial port.
void syscall_serial_writestr(char *str, unsigned int len);

#endif
