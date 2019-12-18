#include "syscall.h"
#include "shell.h"
#include "utils/debug.h"

char greeting[] = "Hi! Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";
unsigned int _cur; // Global cursor position

void shell_init() {
    syscall_register_kb_handler(shell_handle_keypress);
    syscall_fb_clr_scr();
    syscall_fb_write_str(greeting, 0, sizeof(greeting));
    _cur = 80 * 3;
}

void shell_handle_keypress(unsigned char ascii) {
    if (ascii == 0) return;
    syscall_fb_write_str(&ascii, 80 * 2, 1);
    char buf[16] = {0};
    syscall_read_cin(buf, 2);
    syscall_fb_write_str(buf, 80 * 4, 2);
}

void shell_print(const char* str, unsigned int len) {
    unsigned int chars_lost = syscall_fb_write_str(str, _cur, len);
    _cur = _cur + len - chars_lost;
}
