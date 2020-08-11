#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H
#define KEY_LSHIFT 14
#define KEY_RSHIFT 15
#define KEY_BACKSPACE 8
#include "stdint.h"

void keyboard_init();
uint8_t read_scan_code();
uint8_t scan_code_to_ascii(uint8_t scan_code, int is_shift_depressed);
#endif
