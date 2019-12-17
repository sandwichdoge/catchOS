#include "syscall.h"
#include "shell.h"

char greeting[] = "Hi! Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";

void shell_init() {
    syscall_register_kb_handler(shell_handle_keypress);
    syscall_fb_clr_scr();
    syscall_fb_write_str(greeting, 0, sizeof(greeting));
}

void shell_handle_keypress(unsigned char ascii) {
    if (ascii == 0) return;
    syscall_fb_write_str(&ascii, 80 * 2, 1);
}
