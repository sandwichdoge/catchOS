#include "io.h"
#include "keyboard.h"

#define KBD_DATA_PORT 0x60

unsigned char read_scan_code() {
    return inb(KBD_DATA_PORT);
}