#include "builddef.h"
#include "utils/bitmap.h"


public void bitmap_set_bit(char* bitstring, unsigned int bitno) {
    unsigned int byte_no = bitno / 8;
	unsigned int carry_bit = bitno % 8;
	bitstring[byte_no] |= (1 << carry_bit);
}

public void bitmap_clear_bit(char* bitstring, unsigned int bitno) {
	unsigned int byte_no = bitno / 8;
	unsigned int carry_bit = bitno % 8;
    bitstring[byte_no] &= ~(1 << carry_bit);
}

public int bitmap_get_bit(char* bitstring, unsigned int bitno) {
    unsigned int byte_no = bitno / 8;
    unsigned int carry_bit = bitno % 8;
    return (bitstring[byte_no] >> carry_bit) & 1;
}
