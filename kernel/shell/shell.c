#include "syscall.h"
#include "shell.h"
#include "keyboard.h" // For key defs
#include "builddef.h"
#include "utils/string.h"
#include "utils/debug.h"

#define CIN_BUFSZ 256

char msg_hello[] = "Hello!";
char msg_q_name[] = "What is your name? ";
char greeting[] = "Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";

unsigned int SCREEN_WIDTH, SCREEN_HEIGHT;

unsigned int _cur; // Global cursor position
static char _receiving_user_input;

static unsigned char _cin_buf_[CIN_BUFSZ];
unsigned char* _cin;
int _cin_pos;


private void shell_handle_keypress(unsigned char ascii) {
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
        if (ascii == KEY_BACKSPACE) {
            if (_cin_pos == 0) return;
            _cin[_cin_pos--] = '\0';
            _cur--;
            syscall_fb_write_chr('\0', &_cur);
            _cur--;
        } else {
            _cin[_cin_pos++] = ascii;
            syscall_fb_write_chr(ascii, &_cur);
        }
        
        syscall_fb_mov_cursr(_cur - 1);
    }
}

private void shell_init() {
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

private void shell_setpos(unsigned int scrpos){
    _cur = scrpos;
}

private void shell_cout(const char* str, unsigned int len) {
    // Print a string to screen, taking into account linebreaks because framebuffer doesn't know what a linebreak is.
    char *tmp = (char*)str;
    while (len--) {
        if (*tmp == '\n') {
            _cur = _cur + (SCREEN_WIDTH - (_cur % SCREEN_WIDTH)); // Go to start of next line.
            if (_cur >= SCREEN_WIDTH * SCREEN_HEIGHT) { // Scrolldown if screen limit is reached.
                syscall_fb_scroll_down(1);
                _cur -= SCREEN_WIDTH;
            }
        } else { // Normal character, just write it out to screen.
            syscall_fb_write_chr(*tmp, &_cur); // This already automatically scrolls down screen if screen height is reached.
        }
        tmp++;
    }
    syscall_fb_mov_cursr(_cur - 1);
}

private void shell_cin(char* out) {
    _cin_pos = 0;
    _receiving_user_input = 1;
    while (_receiving_user_input) {
        asm("hlt");
    }

    if ((unsigned long)_cin_pos <= sizeof(_cin_buf_)) {
        _memcpy(_cin, out, _cin_pos);
        _memset(_cin, 0, sizeof(_cin_buf_));
    } else {
        // Handle stdin overflow (user enters more than 256 chars)
        _dbg_break();
    }

    _cin_pos = 0;
}

public void shell_main() {
    shell_init();

    shell_cout(msg_hello, sizeof(msg_hello));
    char buf[CIN_BUFSZ] = {0};

    for (;;) {
        _memset(buf, 0, CIN_BUFSZ);
        shell_cout(msg_q_name, _strlen(msg_q_name));
        
        shell_cin(buf);
        shell_cout("\n", 1);
        shell_cout("Cheers ", _strlen("Cheers ")); // \nCheers User\n
        shell_cout(buf, _strlen(buf));
        shell_cout("\n", 1);
    }
}