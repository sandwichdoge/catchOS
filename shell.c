#include "syscall.h"
#include "shell.h"
#include "utils/string.h"
#include "utils/debug.h"

char msg_hello[] = "Hello!";
char msg_q_name[] = "What is your name?";
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
        if (ascii == '\n') {
            _receiving_user_input = 0;
            return;
        }
        syscall_fb_write_str(&ascii, &_cur, 1);
    }
}

void shell_setpos(unsigned int scrpos){
    _cur = scrpos;
}

void shell_print(const char* str, unsigned int len) {
    // TODO parse str, handle linebreak
    syscall_fb_write_str(str, &_cur, len);
}

void shell_cin(const char* out, unsigned int current_scrpos) {
    _cur = current_scrpos;
    _receiving_user_input = 1;
    while (_receiving_user_input) {
    }
    syscall_read_cin(out, CIN_BUFSZ);
}

void shell_main() {
    shell_print(msg_hello, sizeof(msg_hello));
    shell_print(msg_q_name, sizeof(msg_q_name));

    char buf[128] = {0};
    shell_cin(buf, _cur);
    shell_setpos(80 * 4);
    shell_print("Hello \0", _strlen("Hello "));
    shell_print(buf, _strlen(buf));
}