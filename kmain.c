#include "serial.h"
#include "framebuffer.h"

char message[] = "hello";

void kmain() {
    write_cell(2, (char)'x', FB_BLACK, FB_WHITE);
    move_cursor(0, 1);
    write_cell(4, (char)'y', FB_BLACK, FB_WHITE);
    move_cursor(0, 2);
    write_cell(6, (char)'z', FB_BLACK, FB_WHITE);
    move_cursor(0, 3);
}