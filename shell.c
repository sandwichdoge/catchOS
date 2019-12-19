#include "syscall.h"
#include "shell.h"
#include "utils/string.h"
#include "utils/debug.h"

#define CIN_BUFSZ 256

char msg_hello[] = "Hello!";
char msg_q_name[] = "What is your name?";
char greeting[] = "Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";

unsigned int _cur; // Global cursor position
static char _receiving_user_input;

static unsigned char _cin_buf_[CIN_BUFSZ];
unsigned char* _cin;
int _cin_pos;

void shell_init() {
    _cin = _cin_buf_;
    _receiving_user_input = 0;
    _cur = 0;
    syscall_register_kb_handler(shell_handle_keypress);
    syscall_fb_clr_scr();
    syscall_fb_write_str(greeting, &_cur, sizeof(greeting));
    _cur = 80 * 3;
}

void shell_read_cin(char *out, unsigned int len) {
    if (len <= sizeof(_cin_buf_)) {
        _memcpy(_cin, out, len);
        _memset(_cin, 0, sizeof(_cin_buf_));
        _cin_pos = 0;
    }
}

void shell_handle_keypress(unsigned char ascii) {
    if (_receiving_user_input) {
        if (ascii == 0) return;
        if (ascii == '\n') {
            _receiving_user_input = 0;
            return;
        }
        if (_cin_pos >= CIN_BUFSZ) {
            // Handle stdin buffer overflow
            return;
        }

        _cin[_cin_pos++] = ascii;

        syscall_fb_write_str((const char*)&ascii, &_cur, 1);
        syscall_fb_mov_cursr(_cur - 1);
    }
}

void shell_setpos(unsigned int scrpos){
    _cur = scrpos;
}

void shell_print(const char* str, unsigned int len) {
    // TODO parse str, handle linebreak
    syscall_fb_write_str(str, &_cur, len);
}

void shell_cin(char* out) {
    _cin_pos = 0;
    _receiving_user_input = 1;
    while (_receiving_user_input) {
    }
    shell_read_cin(out, CIN_BUFSZ);
}

void shell_main() {
    shell_print(msg_hello, sizeof(msg_hello));
    shell_print(msg_q_name, sizeof(msg_q_name));

    char buf[CIN_BUFSZ] = {0};
    shell_cin(buf);

    shell_setpos(80 * 4);
    shell_print("Cheers \0", _strlen("Cheers "));
    shell_print(buf, _strlen(buf));
}