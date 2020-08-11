#include "bmp.h"

#define BMP_MAGIC_OFF 0
#define BMP_FILESIZE_OFF 2
#define BMP_PIXELARRAY_OFF 10

#define BMP_DIB_HEADER_OFF 14

// OS21XBITMAPHEADER, OS22XBITMAPHEADER
struct BITMAPCOREHEADER {
    uint32_t size;
    uint16_t img_w;
    uint16_t img_h;
    uint16_t planes;
    uint16_t bpp;
} __attribute__((packed));

// Modern bmp
struct BITMAPINFOHEADER {
    uint32_t size;
    uint32_t img_w;
    uint32_t img_h;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
} __attribute__((packed));

static int32_t _get_bmp_dimensions_BITMAPCOREHEADER(struct BITMAPCOREHEADER* dib_header, struct bmp* out) {
    out->w = dib_header->img_w;
    out->h = dib_header->img_h;
    out->bpp = dib_header->bpp;
    return 0;
}

static int32_t _get_bmp_dimensions_BITMAPINFOHEADER(struct BITMAPINFOHEADER* dib_header, struct bmp* out) {
    out->w = dib_header->img_w;
    out->h = dib_header->img_h;
    out->bpp = dib_header->bpp;
    if (dib_header->compression) {
        // printf("Warning. Compression unsupported [%u].\n", dib_header->compression);
        return -1;
    }
    return 0;
}

int libbmp_decode_bmp(void* rawbmp, struct bmp* out) {
    unsigned char* bmp = rawbmp;
    if (bmp[BMP_MAGIC_OFF] != 'B' || bmp[BMP_MAGIC_OFF + 1] != 'M') {
        return -1;  // Error. Not a valid bitmap file.
    }

    uint32_t filesize = *(uint32_t*)(bmp + BMP_FILESIZE_OFF);
    out->filesize = filesize;

    uint32_t pixelarray_offset = *(uint32_t*)(bmp + BMP_PIXELARRAY_OFF);
    out->pixelarray_offset = pixelarray_offset;
    out->pixelarray = bmp + pixelarray_offset;

    uint32_t dib_header_sz = *(uint32_t*)(bmp + BMP_DIB_HEADER_OFF);
    ;
    void* dib_header = bmp + BMP_DIB_HEADER_OFF;
    if (dib_header_sz == 12 || dib_header_sz == 16 || dib_header_sz == 64) {
        _get_bmp_dimensions_BITMAPCOREHEADER(dib_header, out);
    } else {
        _get_bmp_dimensions_BITMAPINFOHEADER(dib_header, out);
    }

    return 0;
}

static uint32_t round_up(uint32_t n, uint32_t alignment) {
    uint32_t ret = n;
    while (ret % alignment != 0) {
        ret++;
    }
    return ret;
}

// https://en.wikipedia.org/wiki/BMP_file_format
void libbmp_get_pixel(struct bmp* bmp, uint32_t x, uint32_t y, struct bmp_pixel* out) {
    if (x > bmp->w || y > bmp->h) {
        return;
    }
    uint32_t row_sz = round_up((bmp->bpp * bmp->w + 31) / 8, 4);  // In bytes

    void* pixelarray = bmp->pixelarray;
    uint32_t pixel_offset = (row_sz * y) + x * (bmp->bpp / 8);
    char* pixel = (char*)pixelarray + pixel_offset;

    switch (bmp->bpp) {
        case 24: {
            uint32_t data = *(uint32_t*)pixel;
            out->a = 0;
            out->b = (data >> 0) & 0xff;
            out->g = (data >> 8) & 0xff;
            out->r = (data >> 16) & 0xff;
            break;
        }
        default: {
            return;
        }
    }
}

void libbmp_get_all_pixels(struct bmp* bmp, struct bmp_pixel* array_out) {
    uint32_t row_sz = round_up((bmp->bpp * bmp->w + 31) / 8, 4);  // In bytes
    void* pixelarray = bmp->pixelarray;
    static uint32_t index = 0;

    for (uint32_t y = 0; y < bmp->h; ++y) {
        for (uint32_t x = 0; x < bmp->w; ++x) {
            uint32_t pixel_offset = (row_sz * y) + x * (bmp->bpp / 8);
            char* pixel = (char*)pixelarray + pixel_offset;
            uint32_t data = *(uint32_t*)pixel;
            switch (bmp->bpp) {
                case 32:
                case 24: {
                    array_out[index].r = (data >> 16) & 0xff;
                    array_out[index].g = (data >> 8) & 0xff;
                    array_out[index++].b = (data >> 0) & 0xff;
                }
            }
        }
    }
}
