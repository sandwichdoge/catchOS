/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"

#include "builddef.h"
#include "drivers/serial.h"
#include "drivers/svga.h"
#include "mmu.h"
#include "utils/debug.h"

static struct rgb_color brush = {0xff, 0xff, 0x0};

private
int32_t syscall_handler(size_t* return_reg, struct cpu_state* cpu) {
    size_t syscall_no = cpu->eax;
    _dbg_log("syscall_no:%u, return register:%x\n", syscall_no, return_reg);
    switch (syscall_no) {
        case SYSCALL_MMAP: {
            void* p = mmu_mmap(cpu->ecx);
            *return_reg = (size_t)p;
            break;
        }
        case SYSCALL_MUNMAP: {
            mmu_munmap((void*)cpu->ecx);
            break;
        }
    }

    return 0;
}

// int 128 (or 0x80). Syscalls may modify eax, ecx, e11.
private
void ISR_SYSCALL(size_t* return_reg, struct cpu_state* regs) {
    syscall_handler(return_reg, regs);
}

void syscall_init() {
    interrupt_register(INT_SYSCALL, ISR_SYSCALL);
}

void syscall_register_kb_handler(void (*kb_handler)(uint8_t c)) {
    _kb_handler_cb = kb_handler;
}

inline uint32_t syscall_fb_get_scr_cols() {
    return svga_get_scr_columns();
}

inline uint32_t syscall_fb_get_scr_rows() {
    return svga_get_scr_rows();
}

inline void syscall_fb_scroll_down(size_t lines) {
    svga_scroll_down(lines);
}

inline void syscall_fb_write_chr(const char c, size_t* scrpos) {
    svga_draw_char_cell(scrpos, c, svga_translate_rgb(brush.r, brush.g, brush.b));
}

inline void syscall_fb_write_str(const char* str, size_t* scrpos, size_t len) {
    svga_write_str(str, scrpos, len, svga_translate_rgb(brush.r, brush.g, brush.b));
}

inline void syscall_fb_brush_set_color(uint8_t r, uint8_t g, uint8_t b) {
    brush.r = r;
    brush.g = g;
    brush.b = b;
}

inline void syscall_fb_draw_rect(const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2) {
    svga_draw_rect(x1, y1, x2, y2, svga_translate_rgb(brush.r, brush.g, brush.b));
}

inline void syscall_fb_clr_cell(size_t* scrpos) {
    svga_clr_cell(scrpos);
}

inline void syscall_fb_clr_scr() {
    svga_clr_scr();
}

inline void syscall_fb_mov_cursr(size_t scrpos) {
    svga_move_cursor(scrpos);
}

inline void syscall_serial_writestr(char* str, size_t len) {
    serial_write(SERIAL_COM1_BASE, str, len);
}
