
#include "builddef.h"

public
void bitmap_set_bit(unsigned char* bitstring, unsigned int bitno) {
    // 0110 0000
    if (!bitstring) return;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = (bitno % 8);
    bitstring[byte_no] |= (1 << carry_bit);
}

public
void bitmap_clear_bit(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = (bitno % 8);
    bitstring[byte_no] &= ~(1 << carry_bit);
}

public
int bitmap_get_bit(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return -1;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = (bitno % 8);
    return (bitstring[byte_no] >> carry_bit) & 1;
}

public
int bitmap_get_bit_reverse(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return -1;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = 7 - (bitno % 8);
    return (bitstring[byte_no] >> carry_bit) & 1;
}

public
int bitmap_find_first_set(unsigned char* bitstring, unsigned int maxbyte) {
    if (!bitstring) return -1;
    int bitno = 0;
    unsigned int leftover = (maxbyte / 4) * 4;  // Dword-aligned bitmap only.

    for (unsigned int i = 0; i < leftover; i += 4) {
        unsigned int* tmp = (unsigned int*)(bitstring + i);
        bitno = __builtin_ffsl(*tmp);

        if (bitno > 0) {
            bitno += i * 8;
            break;
        }
    }

    return bitno - 1;
}

public
void bitmap_toggle_bit(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = (bitno % 8);
    bitstring[byte_no] ^= (1 << carry_bit);
}

public
void bitmap_set_bits(unsigned char* bitstring, unsigned int startbit, unsigned int count) {
    for (unsigned int i = startbit; i < startbit + count; ++i) {
        bitmap_set_bit(bitstring, i);
    }
}
