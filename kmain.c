#include "serial.h"
#include "framebuffer.h"

char message[] = "hello";
char greeting[] = "Hi! Welcome to Thuan's OS!";

void kmain() {
    clr_screen(FB_BLACK);
    write_str(greeting, 0, sizeof(greeting));

    serial_defconfig(SERIAL_COM1_BASE);
}