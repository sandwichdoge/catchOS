#include "io.h"

// I/O ports
#define SERIAL_COM1_BASE 0x3f8

#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

// I/O port commands
// Tells serial port to expect high 8 bits first, then low 8 bits second
#define SERIAL_LINE_ENABLE_DLAB 0x80

// baud = 115200 / divisor
void serial_configure_baud_rate(unsigned short com, unsigned short divisor) {
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00ff); // High 8 bits first
    outb(SERIAL_DATA_PORT(com), divisor & 0x00ff);
}

void serial_configure_line(unsigned short com) {
    /*
     Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     Content: | d | b | prty  | s |  dl |
     Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     d    = Enable/Disable DLAB
     b    = Enable/Disable break control
     prty = Number of parity bits to use
     s    = Stop bits (s = 0 equals 1, s = 1 equals 1.5 or 2)
     dl   = Data length
     */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}
