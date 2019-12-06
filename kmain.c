#include "serial.h"
#include "framebuffer.h"
#include "segmentation.h"

char message[] = "hello";
char greeting[] = "Hi! Welcome to Thuan's OS!";

void kmain() {
    serial_defconfig(SERIAL_COM1_BASE);

    clr_screen(FB_BLACK);
    write_str(greeting, 0, sizeof(greeting));

    //seg_init_gdt();
}