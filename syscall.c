/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"
#include "framebuffer.h"
#include "serial.h"
#include "utils/string.h"
#include "utils/debug.h"

unsigned char _cin_buf_[160];
unsigned char* _cin;
int _cin_pos;

void syscall_init() {
    _cin = _cin_buf_;
    _cin_pos = 0;
}

void syscall_register_kb_handler(void (*kb_handler)(unsigned char c)) {
    _kb_handler_cb = kb_handler;
}

void syscall_read_cin(char *out, unsigned int len) {
    _memcpy(_cin, out, len);
    _memset(_cin, 0, sizeof(_cin));
    _cin_pos = 0;
}

unsigned int syscall_fb_write_str(const char *str, unsigned int scrpos, unsigned int len) {
    return write_str(str, scrpos, len);
}

void syscall_fb_clr_scr() {
    clr_screen(FB_BLACK);
}

void syscall_fb_mov_cursr(unsigned int scrpos) {
    move_cursor(scrpos);
}

void syscall_serial_writestr(char *str, unsigned int len) {
    serial_write(SERIAL_COM1_BASE, str, len);
}
