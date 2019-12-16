#include "framebuffer.h"
#include "serial.h"
#include "syscall.h"
#include "shell.h"

void shell_init() {
    syscall_register_kb_handler(shell_handle_keypress);
}

void shell_handle_keypress(unsigned char ascii) {
    if (ascii == 0) return;
    write_cell(320, ascii, FB_BLACK, FB_WHITE);
}
