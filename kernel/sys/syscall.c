/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"
#include "framebuffer.h"
#include "serial.h"
#include "builddef.h"
#include "utils/debug.h"
#include "mmu.h"

int syscall_handler(unsigned int* return_reg, struct cpu_state* cpu) {
    int syscall_no = cpu->eax;
    _dbg_log("syscall_no:%d, return register:%x\n", syscall_no, return_reg);
    switch (syscall_no) {
        case SYSCALL_MMAP: {
            void* p = mmu_mmap(cpu->ecx);
            *return_reg = (unsigned int)p;
            break;
        }
        case SYSCALL_MUNMAP: {
            mmu_munmap((void*)cpu->ecx);
            break;
        }
    }

    return 0;
}

void syscall_register_kb_handler(void (*kb_handler)(unsigned char c)) {
    _kb_handler_cb = kb_handler;
}

int syscall_fb_get_scr_w() {
    return fb_get_scr_w();
}

int syscall_fb_get_scr_h() {
    return fb_get_scr_h();
}

void syscall_fb_scroll_down(unsigned int lines) {
    scroll_down(lines);
}

void syscall_fb_write_chr(const char c, unsigned int *scrpos) {
    write_chr(c, scrpos);
}

void syscall_fb_write_str(const char *str, unsigned int *scrpos, unsigned int len) {
    write_str(str, scrpos, len);
}

void syscall_fb_clr_scr() {
    clr_screen(FB_BLACK);
}

void syscall_fb_mov_cursr(unsigned int scrpos) {
    move_cursor(scrpos);
}

void syscall_serial_writestr(char *str, unsigned int len) {
    serial_write(SERIAL_COM1_BASE, str, len);
}
