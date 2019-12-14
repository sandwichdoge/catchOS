#include "io.h"
#include "keyboard.h"

#define KBD_DATA_PORT 0x60

unsigned char read_scan_code() {
    return inb(KBD_DATA_PORT);
}

unsigned char sc_table_noshift[256] = {
    0, 27, '1', '2', '3', '4', '5', '6', 
    '7', '8', '9', '0', '-', '=', 8 /*Backspace*/, '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 
    'o', 'p', '[', ']', '\n', 17 /*LCtrl*/, 'a', 's', 
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 
    '\'', '`', 14 /*LShift*/, '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', 'm', ',', '.', '/', 15 /*RShift*/, 0, 
    0 /*LAlt*/, ' ', 26 /*CapsLock*/, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0 /*F10*/, 0 /*NumLock*/, 0 /*ScrollLock*/, 0 /*Home*/, 0 /*Up*/, 
    0 /*PgUp*/, 0 /*Keypad--*/, 0 /*Left*/, 0 /*Keypad-5*/, 0 /*Right*/, 0 /*Keypad-+*/, 0 /*End*/, 0 /*Down*/, 
    0 /*PgDown*/, 0 /*Insert*/, 0 /*Keypad-Del*/, 0, 0, 0, 0 /*F11*/, 0 /*F12*/
};


// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
unsigned char scan_code_to_ascii(unsigned char scan_code) {
    scan_code &= ~(1 << 7);
    return sc_table_noshift[scan_code];
}
