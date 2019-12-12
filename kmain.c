#include "serial.h"
#include "framebuffer.h"
#include "kboot.h"

char message[] = "hello";
char greeting[] = "Hi! Welcome to Thuan's OS!";

void kboot() {
    load_gdt();      // Init gdt for 32-bit memory kernel.
    switch_to_pm();  // Switch to protected mode.
    // switch_to_pm() will call kmain() and never return
}

void kmain() {
    serial_defconfig(SERIAL_COM1_BASE);

    clr_screen(FB_BLACK);
    write_str(greeting, 0, sizeof(greeting));
}
