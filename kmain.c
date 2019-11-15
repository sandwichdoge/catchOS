#include "serial.h"
#include "framebuffer.h"

char message[] = "hello";

void kmain() {
    write_cell(2, (char)'x', 0, 15);
    move_cursor(0, 1);
}