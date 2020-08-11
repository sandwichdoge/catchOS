
#include "bmp.h"
#include "builddef.h"
#include "drivers/keyboard.h"  // For key defs
#include "drivers/svga.h"
#include "kinfo.h"
#include "mmu.h"
#include "panic.h"
#include "power/shutdown_reboot.h"
#include "syscall.h"
#include "tasks.h"
#include "timer.h"
#include "utils/debug.h"

#define CIN_BUFSZ 256
#define MSG_HELP \
    "help\n\
uptime\n\
program\n\
rei.bmp\n\
tests\n\
clear\n\
reboot\n\
shutdown"

char* greeting =
    " ______     ______     ______   ______     __  __     ______     ______    \n"
    "/\\  ___\\   /\\  __ \\   /\\__  _\\ /\\  ___\\   /\\ \\_\\ \\   /\\  __ \\   /\\  ___\\   \n"
    "\\ \\ \\____  \\ \\  __ \\  \\/_/\\ \\/ \\ \\ \\____  \\ \\  __ \\  \\ \\ \\/\\ \\  \\ \\___  \\  \n"
    " \\ \\_____\\  \\ \\_\\ \\_\\    \\ \\_\\  \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_____\\  \\/\\_____\\ \n"
    "  \\/_____/   \\/_/\\/_/     \\/_/   \\/_____/   \\/_/\\/_/   \\/_____/   \\/_____/ \n"
    "                                                                           \n";

// Screen size in characters instead of in pixels.
size_t SCREEN_COLUMNS, SCREEN_ROWS;

size_t _cur;  // Global cursor position
char _receiving_user_input;

static uint8_t _cin_buf_[CIN_BUFSZ];
uint8_t* _cin;
int _cin_pos;

private
void shell_handle_keypress(uint8_t ascii) {
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
void shell_cout(const char* str) {
    // Print a string to screen, taking into account linebreaks because framebuffer doesn't know what a linebreak is.
    char* tmp = (char*)str;
    while (*tmp) {
        if (*tmp == '\n') {
            _cur = _cur + (SCREEN_COLUMNS - (_cur % SCREEN_COLUMNS));  // Go to start of next line.
            if (_cur >= SCREEN_COLUMNS * SCREEN_ROWS) {            // Scrolldown if screen limit is reached.
                syscall_fb_scroll_down(1);
                _cur -= SCREEN_COLUMNS;
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
        panic();
    }

    _cin_pos = 0;
}

private
void shell_init() {
    keyboard_init();
    _cin = _cin_buf_;
    _receiving_user_input = 0;
    _cur = 0;
    SCREEN_COLUMNS = syscall_fb_get_scr_cols();
    SCREEN_ROWS = syscall_fb_get_scr_rows();
    syscall_register_kb_handler(shell_handle_keypress);
    syscall_fb_clr_scr();
    _cur = 0;
    shell_cout(greeting);
}

private
size_t shell_gettime() { return getticks(); }

private
int call_user_module() {
    struct multiboot_tag_module* mod = &(get_kernel_info()->mods[0]);
    _dbg_log("Call user module at 0x%x\n", mod->mod_start);

    if (mod->size > 0) {
        size_t prog_addr = (mod->mod_start + 0x0);
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
        syscall_fb_write_str(msg, (size_t*)screenpos, _strlen(msg) + 1);
        delay(500);
    }
}

private
void run_tests() {
    _dbg_log("Running tests\n");
    syscall_fb_brush_set_color(0xff, 0, 0);
    syscall_fb_draw_rect(300, 200, 350, 250);
    syscall_fb_brush_set_color(0x00, 0xff, 0x0);
    syscall_fb_draw_rect(350, 200, 400, 250);
    syscall_fb_brush_set_color(0x00, 0x0, 0xff);
    syscall_fb_draw_rect(400, 200, 450, 250);
    syscall_fb_brush_set_color(0xff, 0xff, 0x0);
    static size_t pos1 = 85 * 7;
    static size_t pos2 = 85 * 8;
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
        size_t ticks = shell_gettime() * 10;
        _int_to_str(ticksbuf, sizeof(ticksbuf), ticks);
        shell_cout(ticksbuf);
        shell_cout("\n");
    } else if (_strncmp(cmd, "help", _strlen("help")) == 0) {
        shell_cout(MSG_HELP);
        shell_cout("\n");
    } else if (_strncmp(cmd, "program", _strlen("program")) == 0) {
        int ret = call_user_module();
        static char ret_s[12];
        _memset(ret_s, 0, sizeof(ret_s));
        _int_to_str(ret_s, sizeof(ret_s), ret);
        shell_cout(ret_s);
        shell_cout("\n");
    } else if (_strncmp(cmd, "rei.bmp", _strlen("rei.bmp")) == 0) {
        struct kinfo* kinfo = get_kernel_info();
        void* img_rei = (void*)kinfo->mods[1].mod_start;
        struct bmp bmp_rei;
        libbmp_decode_bmp(img_rei, &bmp_rei);

        struct bmp_pixel* pix = mmu_mmap(bmp_rei.h * bmp_rei.w * sizeof(struct bmp_pixel));
        libbmp_get_all_pixels(&bmp_rei, pix);
        uint32_t index = 0;
        for (uint32_t i = 0; i < bmp_rei.h; ++i) {
            for (uint32_t j = 0; j < bmp_rei.w; ++j) {
                uint32_t color = svga_translate_rgb(pix[index].r, pix[index].g, pix[index].b);
                index++;
                svga_draw_pixel(450 + j, 100 + bmp_rei.h - i, color);
            }
        }
        mmu_munmap(pix);
    } else if (_strncmp(cmd, "tests", _strlen("tests")) == 0) {
        run_tests();
    } else if (_strncmp(cmd, "reboot", _strlen("reboot")) == 0) {
        reboot();
    } else if (_strncmp(cmd, "shutdown", _strlen("shutdown")) == 0) {
        shutdown();
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
        shell_cout(">");

        shell_cin(buf);
        shell_cout("\n");

        shell_handle_cmd(buf);
    }
}
