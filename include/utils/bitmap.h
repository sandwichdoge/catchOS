#ifndef INCLUDE_BITMAP_H
#define INCLUDE_BITMAP_H
#ifdef __cplusplus
extern "C" {
#endif

void bitmap_set_bit(unsigned char* bitstring, unsigned int bitno);
void bitmap_clear_bit(unsigned char* bitstring, unsigned int bitno);
int bitmap_get_bit(unsigned char* bitstring, unsigned int bitno);
int bitmap_get_bit_reverse(unsigned char* bitstring, unsigned int bitno);
void bitmap_toggle_bit(unsigned char* bitstring, unsigned int bitno);

void bitmap_set_bits(unsigned char* bitstring, unsigned int startbit, unsigned int count);
// First position of first set bit, starting from left side. DWORD-aligned bitmaps only.
int bitmap_find_first_set(unsigned char* bitstring, unsigned int maxlen);

#ifdef __cplusplus
}
#endif
#endif
