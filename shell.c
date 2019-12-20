#include "syscall.h"
#include "shell.h"
#include "utils/string.h"
#include "utils/debug.h"

#define CIN_BUFSZ 256

char msg_hello[] = "Hello!";
char msg_q_name[] = "What is your name? ";
char greeting[] = "Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";

int SCREEN_WIDTH, SCREEN_HEIGHT;

unsigned int _cur; // Global cursor position
static char _receiving_user_input;

static unsigned char _cin_buf_[CIN_BUFSZ];
unsigned char* _cin;
int _cin_pos;

void shell_init() {
    _cin = _cin_buf_;
    _receiving_user_input = 0;
    _cur = 0;
    SCREEN_WIDTH = syscall_fb_get_scr_w();
    SCREEN_HEIGHT = syscall_fb_get_scr_h();
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

void shell_cout(const char* str, unsigned int len) {
    // TODO parse str, handle linebreak
    char *tmp = str;
    while (len--) {
        if (*tmp == '\n') {
            _cur = _cur + (SCREEN_WIDTH - (_cur % SCREEN_WIDTH)); // Next line
            if (_cur >= SCREEN_WIDTH * SCREEN_HEIGHT) {
                syscall_fb_scroll_down(1);
                _cur -= SCREEN_WIDTH;
            }
        } else {
            syscall_fb_write_chr(*tmp, &_cur);
        }
        tmp++;
    }
    syscall_fb_mov_cursr(_cur - 1);
}

void shell_cin(char* out) {
    _cin_pos = 0;
    _receiving_user_input = 1;
    while (_receiving_user_input) {
    }
    shell_read_cin(out, _cin_pos);
    _cin_pos = 0;
}

char msg_cheers[] = "Cheers ";

void shell_main() {
    shell_cout(msg_hello, sizeof(msg_hello));
    char buf[CIN_BUFSZ] = {0};

    for (;;) {
        _memset(buf, 0, CIN_BUFSZ);
        shell_cout(msg_q_name, _strlen(msg_q_name));
        shell_cin(buf);
        shell_cout("\n", 1);
        shell_cout(msg_cheers, _strlen(msg_cheers)); // \nCheers User\n
        shell_cout(buf, _strlen(buf));
        shell_cout("\n", 1);
    }

}