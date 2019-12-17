/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"
#include "framebuffer.h"
#include "serial.h"

void syscall_register_kb_handler(void (*kb_handler)(unsigned char c)) {
    _kb_handler_cb = kb_handler;
}

void syscall_fb_write_str(const char *str, unsigned int scrpos, unsigned int len) {
    write_str(str, scrpos, len);
}

void syscall_serial_writestr(char *str, unsigned int len) {
    serial_write(SERIAL_COM1_BASE, str, len);
}
