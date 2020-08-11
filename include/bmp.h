#ifndef INCLUDE_LIBBMP_H
#define INCLUDE_LIBBMP_H

#include "stdint.h"

struct bmp {
    int filesize;
    uint32_t w;
    uint32_t h;
    uint32_t bpp;
    uint32_t pixelarray_offset;
    void* pixelarray;
};

struct bmp_pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

int libbmp_decode_bmp(void* rawbmp, struct bmp* out);
void libbmp_get_pixel(struct bmp* bmp, uint32_t x, uint32_t y, struct bmp_pixel* out);
void libbmp_get_all_pixels(struct bmp* bmp, struct bmp_pixel* array_out);

#endif
