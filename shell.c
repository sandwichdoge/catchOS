#include "syscall.h"
#include "shell.h"
#include "utils/debug.h"

char greeting[] = "Hi! Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";
unsigned int _cur; // Global cursor position
static char _receiving_user_input;

void shell_init() {
    _receiving_user_input = 0;
    _cur = 0;
    syscall_register_kb_handler(shell_handle_keypress);
    syscall_fb_clr_scr();
    syscall_fb_write_str(greeting, &_cur, sizeof(greeting));
    _cur = 80 * 3;
}

void shell_handle_keypress(unsigned char ascii) {
    if (_receiving_user_input) {
        if (ascii == 0) return;
        syscall_fb_write_str(&ascii, &_cur, 1);
    }
}

void shell_print(const char* str, unsigned int len) {
    syscall_fb_write_str(str, &_cur, len);
}

void shell_cin(const char* out, unsigned int current_scrpos) {
    _receiving_user_input = 1;
    syscall_read_cin(out, 2);
    _receiving_user_input = 0;
}

void shell_main() {
    char buf[128];
    shell_cin(buf, _cur);
}