#include "syscall.h"
#include "shell.h"

void shell_init() {
    syscall_register_kb_handler(shell_handle_keypress);
}

void shell_handle_keypress(unsigned char ascii) {
    if (ascii == 0) return;
    syscall_fb_write_str(&ascii, 80 * 2, 1);
}
