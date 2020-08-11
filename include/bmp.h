#ifndef INCLUDE_LIBBMP_H
#define INCLUDE_LIBBMP_H

struct bmp {
    int filesize;
    unsigned int w;
    unsigned int h;
    unsigned int bpp;
    unsigned int pixelarray_offset;
    void* pixelarray;
};

struct bmp_pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

int libbmp_decode_bmp(void* rawbmp, struct bmp* out);
void libbmp_get_pixel(struct bmp* bmp, unsigned int x, unsigned int y, struct bmp_pixel* out);
void libbmp_get_all_pixels(struct bmp* bmp, struct bmp_pixel* array_out);

#endif
