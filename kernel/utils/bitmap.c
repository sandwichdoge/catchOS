
#include "builddef.h"

public
void bitmap_set_bit(unsigned char* bitstring, unsigned int bitno) {
    // 0110 0000
    if (!bitstring) return;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = 7 - (bitno % 8);
    bitstring[byte_no] |= (1 << carry_bit);
}

public
void bitmap_clear_bit(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = 7 - (bitno % 8);
    bitstring[byte_no] &= ~(1 << carry_bit);
}

public
int bitmap_get_bit(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return -1;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = 7 - (bitno % 8);
    return (bitstring[byte_no] >> carry_bit) & 1;
}

public
void bitmap_toggle_bit(unsigned char* bitstring, unsigned int bitno) {
    if (!bitstring) return;
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = 7 - (bitno % 8);
    bitstring[byte_no] ^= (1 << carry_bit);
}

public
void bitmap_set_bits(unsigned char* bitstring, unsigned int startbit, unsigned int count) {
    for (unsigned int i = startbit; i < startbit + count; ++i) {
        bitmap_set_bit(bitstring, i);
    }
}
