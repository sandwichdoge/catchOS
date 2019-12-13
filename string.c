#include "string.h"

unsigned int _strlen(char *s) {
    unsigned int ret = 0;
    while (*(s++)) {
        ret++;
    }
    return ret;
}

void _memset(char *dst, char c, unsigned int len) {
    while (len--) {
        *(dst++) = c;
    }
}

void _memcpy(char *src, char *dst, unsigned int len) {
    while (len--) {
        *(dst++) = *(src++);
    }
}