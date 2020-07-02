#ifndef INCLUDE_BITMAP_H
#define INCLUDE_BITMAP_H
#ifdef __cplusplus
extern "C" {
#endif

void bitmap_set_bit(unsigned char* bitstring, unsigned int bitno);
void bitmap_clear_bit(unsigned char* bitstring, unsigned int bitno);
int bitmap_get_bit(unsigned char* bitstring, unsigned int bitno);
void bitmap_toggle_bit(unsigned char* bitstring, unsigned int bitno);

void bitmap_set_bits(unsigned char* bitstring, unsigned int startbit, unsigned int count);

#ifdef __cplusplus
}
#endif
#endif
