#ifndef INCLUDE_BITMAP_H
#define INCLUDE_BITMAP_H
#ifdef __cplusplus
extern "C" {
#endif

void bitmap_set_bit(char* bitstring, unsigned int bitno);
void bitmap_clear_bit(char* bitstring, unsigned int bitno);
int bitmap_get_bit(char* bitstring, unsigned int bitno);

#ifdef __cplusplus
}
#endif
#endif