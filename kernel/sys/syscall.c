/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"

#include "builddef.h"
#include "drivers/serial.h"
#include "drivers/svga.h"
#include "interrupt.h"
#include "mmu.h"
#include "utils/debug.h"

static struct rgb_color brush = {0xff, 0xff, 0x0};

private
int syscall_handler(unsigned int *return_reg, struct cpu_state *cpu) {
    int syscall_no = cpu->eax;
    _dbg_log("syscall_no:%d, return register:%x\n", syscall_no, return_reg);
    switch (syscall_no) {
        case SYSCALL_MMAP: {
            void *p = mmu_mmap(cpu->ecx);
            *return_reg = (unsigned int)p;
            break;
        }
        case SYSCALL_MUNMAP: {
            mmu_munmap((void *)cpu->ecx);
            break;
        }
    }

    return 0;
}

// int 128 (or 0x80). Syscalls may modify eax, ecx, e11.
private
void ISR_SYSCALL(unsigned int *return_reg, struct cpu_state *regs) { syscall_handler(return_reg, regs); }

void syscall_init() { interrupt_register(INT_SYSCALL, ISR_SYSCALL); }

void syscall_register_kb_handler(void (*kb_handler)(unsigned char c)) { _kb_handler_cb = kb_handler; }

int syscall_fb_get_scr_w() { return svga_get_scr_columns(); }

int syscall_fb_get_scr_h() { return svga_get_scr_rows(); }

void syscall_fb_scroll_down(unsigned int lines) { svga_scroll_down(lines); }

void syscall_fb_write_chr(const char c, unsigned int *scrpos) { svga_draw_char_cell(scrpos, c, svga_translate_rgb(brush.r, brush.g, brush.b)); }

void syscall_fb_write_str(const char *str, unsigned int *scrpos, unsigned int len) {
    svga_write_str(str, scrpos, len, svga_translate_rgb(brush.r, brush.g, brush.b));
}

void syscall_fb_brush_set_color(unsigned char r, unsigned char g, unsigned char b) {
    brush.r = r;
    brush.g = g;
    brush.b = b;
}

void syscall_fb_draw_rect(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2) {
    svga_draw_rect(x1, y1, x2, y2, svga_translate_rgb(brush.r, brush.g, brush.b));
}

void syscall_fb_clr_cell(unsigned int *scrpos) { svga_clr_cell(scrpos); }

void syscall_fb_clr_scr() { svga_clr_scr(); }

void syscall_fb_mov_cursr(unsigned int scrpos) { svga_move_cursor(scrpos); }

void syscall_serial_writestr(char *str, unsigned int len) { serial_write(SERIAL_COM1_BASE, str, len); }
