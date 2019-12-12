#include "serial.h"
#include "framebuffer.h"
#include "kboot.h"
#include "pic.h"

char greeting[] = "Hi! Welcome to Thuan's OS! You're now in Protected Mode";
char msg_true[] = "True";
char msg_false[] = "False";

void test_memory_32bit_mode() {
    volatile unsigned char *p = (volatile unsigned char *)3000000; // 32MB for testing 32-bit mode
    *p = 55;

    if (*p == 55) {
        write_str(msg_true, 160, sizeof(msg_true));
    } else {
        write_str(msg_false, 160, sizeof(msg_true));
    }
}

void kmain() {
    serial_defconfig(SERIAL_COM1_BASE);

    clr_screen(FB_BLACK);
    write_str(greeting, 0, sizeof(greeting));

    test_memory_32bit_mode();

    pic_init();
}
