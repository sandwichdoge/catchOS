#include "io.h"

// I/O ports

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

void serial_configure_line(unsigned short com, unsigned char conf) {
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
    // 0x03 for default conf
    outb(SERIAL_LINE_COMMAND_PORT(com), conf);
}

void serial_configure_buffer(unsigned short com, unsigned char conf) {
    // Enable FIFO buffer, clear both rx/tx FIFO queues, use 14 bytes queue size.
    // 0xc7 for default conf
    outb(SERIAL_FIFO_COMMAND_PORT(com), conf);
}

void serial_configure_modem(unsigned short com, unsigned char conf) {
    // Ready To Transmit (RTS) and Data Terminal Ready (DTR) pins should be 1.
    // We don't enable interrupts because we won't be receiving data.
    outb(SERIAL_MODEM_COMMAND_PORT(com), conf);
}

int serial_is_transmit_fifo_empty(unsigned int com) {
    // 0x20 = 0010 0000
    return inb(SERIAL_LINE_STATUS_PORT(com) & 0x20);
}

// Spinning as long as the internal fifo isn't empty, then writing data to data I/O port
void serial_write(unsigned short com, char *data, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        while (serial_is_transmit_fifo_empty(com) == 0);
        outb(SERIAL_DATA_PORT(com), data[i]);
    }
}

void serial_defconfig(unsigned short com) {
    serial_configure_baud_rate(com, 0x01); // 115200
    serial_configure_line(com, 0x03);
    serial_configure_buffer(com, 0x07);
    serial_configure_modem(com, 0x03);
}