#include "bmp.h"
#include "utils/debug.h"

#define BMP_MAGIC_OFF 0
#define BMP_FILESIZE_OFF 2
#define BMP_PIXELARRAY_OFF 10

#define BMP_DIB_HEADER_OFF 14

// OS21XBITMAPHEADER, OS22XBITMAPHEADER
struct BITMAPCOREHEADER {
    unsigned int size;
    unsigned short img_w;
    unsigned short img_h;
    unsigned short planes;
    unsigned short bpp;
}__attribute__((packed));

// Modern bmp
struct BITMAPINFOHEADER {
    unsigned int size;
    unsigned int img_w;
    unsigned int img_h;
    unsigned short planes;
    unsigned short bpp;
    unsigned int compression;
}__attribute__((packed));

static int _get_bmp_dimensions_BITMAPCOREHEADER(struct BITMAPCOREHEADER* dib_header, struct bmp* out) {
    out->w = dib_header->img_w;
    out->h = dib_header->img_h;
    out->bpp = dib_header->bpp;
    return 0;
}

static int _get_bmp_dimensions_BITMAPINFOHEADER(struct BITMAPINFOHEADER* dib_header, struct bmp* out) {
    out->w = dib_header->img_w;
    out->h = dib_header->img_h;
    out->bpp = dib_header->bpp;
    if (dib_header->compression) {
        //printf("Warning. Compression unsupported [%u].\n", dib_header->compression);
        return -1;
    }
    return 0;
}

int libbmp_decode_bmp(void* rawbmp, struct bmp* out) {
    unsigned char* bmp = rawbmp;
    if (bmp[BMP_MAGIC_OFF] != 'B' || bmp[BMP_MAGIC_OFF + 1] != 'M') {
        return -1;  // Error. Not a valid bitmap file.
    }

    unsigned int filesize = *(unsigned int*)(bmp + BMP_FILESIZE_OFF);
    out->filesize = filesize;

    unsigned int pixelarray_offset = *(unsigned int*)(bmp + BMP_PIXELARRAY_OFF);
    out->pixelarray_offset = pixelarray_offset;
    out->pixelarray = bmp + pixelarray_offset;

    unsigned int dib_header_sz = *(unsigned int*)(bmp + BMP_DIB_HEADER_OFF);;    
    void* dib_header = bmp + BMP_DIB_HEADER_OFF;
    if (dib_header_sz == 12 || dib_header_sz == 16 || dib_header_sz == 64) {
        _get_bmp_dimensions_BITMAPCOREHEADER(dib_header, out);
    } else {
        _get_bmp_dimensions_BITMAPINFOHEADER(dib_header, out);
    }

    return 0;
}

static unsigned int round_up(unsigned int n, unsigned int alignment) {
    unsigned int ret = n;
    while (ret % alignment != 0) {
        ret++;
    }
    return ret;
}

// https://en.wikipedia.org/wiki/BMP_file_format
void libbmp_get_pixel(struct bmp* bmp, unsigned int x, unsigned int y, struct bmp_pixel* out) {
    if (x > bmp->w || y > bmp->h) {
        return;
    }
    unsigned int row_sz = round_up((bmp->bpp * bmp->w + 31) / 8, 4);   // In bytes

    void* pixelarray = bmp->pixelarray;
    unsigned int pixel_offset = (row_sz * y) + x * (bmp->bpp / 8);
    char* pixel = (char*)pixelarray + pixel_offset;

    switch (bmp->bpp) {
        case 24: {
            unsigned int data = *(unsigned int*)pixel;
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
    unsigned int row_sz = round_up((bmp->bpp * bmp->w + 31) / 8, 4);   // In bytes
    void* pixelarray = bmp->pixelarray;
    static unsigned int index = 0;

    for (unsigned int y = 0; y < bmp->h; ++y) {
        for (unsigned int x = 0; x < bmp->w; ++x) {
            unsigned int pixel_offset = (row_sz * y) + x * (bmp->bpp / 8);
            char* pixel = (char*)pixelarray + pixel_offset;
            unsigned int data = *(unsigned int*)pixel;
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
