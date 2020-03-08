/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"
#include "framebuffer.h"
#include "serial.h"
#include "builddef.h"
#include "utils/debug.h"

void syscall_register_kb_handler(void (*kb_handler)(unsigned char c)) {
    _kb_handler_cb = kb_handler;
}

int syscall_fb_get_scr_w() {
    return fb_get_scr_w();
}

int syscall_fb_get_scr_h() {
    return fb_get_scr_h();
}

void syscall_fb_scroll_down(unsigned int lines) {
    scroll_down(lines);
}

void syscall_fb_write_chr(const char c, unsigned int *scrpos) {
    write_chr(c, scrpos);
}

void syscall_fb_write_str(const char *str, unsigned int *scrpos, unsigned int len) {
    write_str(str, scrpos, len);
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
