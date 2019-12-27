#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H
#define KEY_LSHIFT 14
#define KEY_RSHIFT 15
#define KEY_BACKSPACE 8

unsigned char read_scan_code();
unsigned char scan_code_to_ascii(unsigned char scan_code, int is_shift_depressed);
#endif