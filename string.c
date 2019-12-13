#include "string.h"

unsigned int _strlen(char *s) {
    unsigned int ret = 0;
    while (*(s++)) {
        ret++;
    }
    return ret;
}

int _strncmp(char *s1, char *s2, unsigned int len) {
    // Probably gonna hate myself for this later.
    while (len--) {
        if (*s1 == 0 || *s2 == 0) break;
        if (*(s1++) != *(s2++)) {
            s1--;
            s2--;
            break;
        }
    }
    return (*s1 - *s2);
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
