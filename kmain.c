#include "serial.h"
#include "framebuffer.h"
#include "interrupt.h"
#include "pic.h"
#include "debug.h"

char greeting[] = "Hi! Welcome to Thuan's OS! You're now in 32-bit Protected Mode.\0";
char msg_memtest_success[] = "Memtest success.\0";
char msg_memtest_failed[] = "Memtest failed.\0";

void test_memory_32bit_mode() {
    volatile unsigned char *p = (volatile unsigned char *)3000000; // 32MB for testing 32-bit mode
    *p = 55;

    if (*p == 55) {
        write_cstr(msg_memtest_success, 160);
    } else {
        write_cstr(msg_memtest_failed, 160);
    }
}

void kmain() {
    serial_defconfig(SERIAL_COM1_BASE);

    clr_screen(FB_BLACK);
    write_cstr(greeting, 0);

    test_memory_32bit_mode();

    interrupt_init_idt();
    pic_init();    

    _dbg_break();
    while (1) {
        
    }
}
