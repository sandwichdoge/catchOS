#include "controller.h"
#include "framebuffer.h"
#include "serial.h"


void controller_handle_keypress(unsigned char ascii) {
    if (ascii == 0) return;
    write_cell(320, ascii, FB_BLACK, FB_WHITE);
}