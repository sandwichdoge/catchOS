#include "shell.h"

#include "builddef.h"
#include "drivers/keyboard.h"  // For key defs
#include "kinfo.h"
#include "multiboot.h"
#include "power/shutdown_reboot.h"
#include "syscall.h"
#include "tasks.h"
#include "timer.h"
#include "utils/debug.h"
#include "utils/string.h"

#define CIN_BUFSZ 256
#define MSG_HELP \
    "help\n\
uptime\n\
program\n\
tests\n\
clear\n\
reboot\n\
2200"

char* greeting =
" ______     ______     ______   ______     __  __     ______     ______    \n"
"/\\  ___\\   /\\  __ \\   /\\__  _\\ /\\  ___\\   /\\ \\_\\ \\   /\\  __ \\   /\\  ___\\   \n"
"\\ \\ \\____  \\ \\  __ \\  \\/_/\\ \\/ \\ \\ \\____  \\ \\  __ \\  \\ \\ \\/\\ \\  \\ \\___  \\  \n"
" \\ \\_____\\  \\ \\_\\ \\_\\    \\ \\_\\  \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_____\\  \\/\\_____\\ \n"
"  \\/_____/   \\/_/\\/_/     \\/_/   \\/_____/   \\/_/\\/_/   \\/_____/   \\/_____/ \n"
"                                                                           \n";

unsigned int SCREEN_WIDTH, SCREEN_HEIGHT;

unsigned int _cur;  // Global cursor position
static char _receiving_user_input;

static unsigned char _cin_buf_[CIN_BUFSZ];
unsigned char* _cin;
int _cin_pos;

private
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
        if (ascii == KEY_BACKSPACE) {
            if (_cin_pos == 0) return;
            _cin[_cin_pos--] = '\0';
            _cur--;
            syscall_fb_clr_cell(&_cur);
            _cur--;
        } else {
            _cin[_cin_pos++] = ascii;
            syscall_fb_write_chr(ascii, &_cur);
        }

        syscall_fb_mov_cursr(_cur - 1);
    }
}

private
void shell_cout(const char* str, unsigned int len) {
    // Print a string to screen, taking into account linebreaks because framebuffer doesn't know what a linebreak is.
    char* tmp = (char*)str;
    while (len--) {
        if (*tmp == '\n') {
            _cur = _cur + (SCREEN_WIDTH - (_cur % SCREEN_WIDTH));  // Go to start of next line.
            if (_cur >= SCREEN_WIDTH * SCREEN_HEIGHT) {            // Scrolldown if screen limit is reached.
                syscall_fb_scroll_down(1);
                _cur -= SCREEN_WIDTH;
            }
        } else {                                // Normal character, just write it out to screen.
            syscall_fb_write_chr(*tmp, &_cur);  // This already automatically scrolls down screen if screen height is reached.
        }
        tmp++;
    }
    syscall_fb_mov_cursr(_cur - 1);
}

private
void shell_cin(char* out) {
    _cin_pos = 0;
    _receiving_user_input = 1;
    while (_receiving_user_input) {
        asm("hlt");
    }

    if ((unsigned long)_cin_pos <= sizeof(_cin_buf_)) {
        _memcpy(out, (char*)_cin, _cin_pos);
        _memset(_cin, 0, sizeof(_cin_buf_));
    } else {
        // Handle stdin overflow (user enters more than 256 chars)
        _dbg_break();
    }

    _cin_pos = 0;
}

private
void shell_init() {
    _cin = _cin_buf_;
    _receiving_user_input = 0;
    _cur = 0;
    SCREEN_WIDTH = syscall_fb_get_scr_w();
    SCREEN_HEIGHT = syscall_fb_get_scr_h();
    syscall_register_kb_handler(shell_handle_keypress);
    // syscall_fb_clr_scr();
    _cur = 0;
    shell_cout(greeting, _strlen(greeting));
    syscall_fb_brush_set_color(0xff, 0x0, 0x0);
    syscall_fb_draw_rect(300, 200, 350, 250);
    syscall_fb_brush_set_color(0x00, 0xff, 0x0);
    syscall_fb_draw_rect(350, 200, 400, 250);
    syscall_fb_brush_set_color(0x00, 0x0, 0xff);
    syscall_fb_draw_rect(400, 200, 450, 250);
    syscall_fb_brush_set_color(0xff, 0xff, 0x0);
}

private
unsigned int shell_gettime() { return getticks(); }

private
int call_user_module() {
    struct multiboot_tag_module* mod = &(get_kernel_info()->mod);
    _dbg_log("Call user module at 0x%x\n", mod->mod_start);

    if (mod->size > 0) {
        unsigned int prog_addr = (mod->mod_start + 0x0);
        typedef void (*call_module_t)(void);
        call_module_t start_program = (call_module_t)prog_addr;

        start_program();
        register int ret asm("eax");
        return ret;
    } else {
        return 0;
    }
}

private
void test_multitasking(void* screenpos) {
    _dbg_log("Test multitasking..\n");
    char msg[16];
    for (int i = 0; i < 10; ++i) {
        _memset(msg, 0, sizeof(msg));
        _int_to_str(msg, sizeof(msg), shell_gettime());
        syscall_fb_write_str(msg, (unsigned int*)screenpos, _strlen(msg) + 1);
        delay(500);
    }
}

private
void run_tests() {
    _dbg_log("Running tests\n");
    static unsigned int pos1 = 85 * 7;
    static unsigned int pos2 = 85 * 8;
    struct task_struct* task1 = task_new(test_multitasking, &pos1, 4096 * 2, 5);
    struct task_struct* task2 = task_new(test_multitasking, &pos2, 4096 * 2, 5);
    task_detach(task1);
    task_detach(task2);
}

private
void shell_handle_cmd(char* cmd) {
    if (_strncmp(cmd, "uptime", _strlen("uptime")) == 0) {
        static char ticksbuf[12];
        _memset(ticksbuf, 0, sizeof(ticksbuf));
        unsigned int ticks = shell_gettime() * 10;
        _int_to_str(ticksbuf, sizeof(ticksbuf), ticks);
        shell_cout(ticksbuf, _strlen(ticksbuf));
        shell_cout("\n", 1);
    } else if (_strncmp(cmd, "help", _strlen("help")) == 0) {
        shell_cout(MSG_HELP, _strlen(MSG_HELP));
        shell_cout("\n", 1);
    } else if (_strncmp(cmd, "program", _strlen("program")) == 0) {
        _dbg_break();
        int ret = call_user_module();
        static char ret_s[12];
        _memset(ret_s, 0, sizeof(ret_s));
        _int_to_str(ret_s, sizeof(ret_s), ret);
        shell_cout(ret_s, _strlen(ret_s));
        shell_cout("\n", 1);
    } else if (_strncmp(cmd, "tests", _strlen("tests")) == 0) {
        run_tests();
    } else if (_strncmp(cmd, "reboot", _strlen("reboot")) == 0) {
        reboot();
    } else if (_strncmp(cmd, "clear", _strlen("clear")) == 0) {
        syscall_fb_clr_scr();
        _cur = 0;
    } else {
    }
}

public
void shell_main(void* unused) {
    shell_init();
    char buf[CIN_BUFSZ];
    _memset(buf, 0, CIN_BUFSZ);

    for (;;) {
        _memset(buf, 0, CIN_BUFSZ);
        shell_cout(">", 1);

        shell_cin(buf);
        shell_cout("\n", 1);

        shell_handle_cmd(buf);
    }
}
