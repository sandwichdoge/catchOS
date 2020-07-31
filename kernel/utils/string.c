#ifdef __cplusplus
extern "C" {
#endif
#include "utils/string.h"

#include "builddef.h"

public
unsigned int _strlen_s(char *s) {
    unsigned int ret = 0;
    while (*(s++)) {
        ret++;
    }
    return ret;
}

unsigned int _strlen_u(unsigned char *s) {
    unsigned int ret = 0;
    while (*(s++)) {
        ret++;
    }
    return ret;
}

public
int _strncmp(char *s1, char *s2, unsigned int n) {
    register unsigned char u1, u2;
    while (n-- > 0) {
        u1 = (unsigned char) *s1++;
        u2 = (unsigned char) *s2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }
    return 0;
}

public
void _memset_s(char *dst, char c, unsigned int len) {
    while (len--) {
        *(dst++) = c;
    }
}

public
void _memset_u(unsigned char *dst, char c, unsigned int len) {
    while (len--) {
        *(dst++) = c;
    }
}

public
void _memcpy(void *dst, void *src, unsigned int len) {
    char *d = dst;
    char *s = src;
    while (len--) {
        *(d++) = *(s++);
    }
}

public
int _strcpy_s(char *dst, char *src) {
    int ret = 0;
    while (*src != '\0') {
        *(dst++) = *(src++);
        ret++;
    }
    return ret;
}

public
int _strcpy_u(unsigned char *dst, char *src) {
    int ret = 0;
    while (*src != '\0') {
        *(dst++) = *(src++);
        ret++;
    }
    return ret;
}

public
int _strncpy_s(char *dst, char *src, unsigned int maxlen) {
    int ret = 0;
    while (*src != '\0' && ret < (int)maxlen) {
        *(dst++) = *(src++);
        ret++;
    }
    return ret;
}

public
int _strncpy_u(unsigned char *dst, unsigned char *src, unsigned int maxlen) {
    int ret = 0;
    while (*src != '\0' && ret < (int)maxlen) {
        *(dst++) = *(src++);
        ret++;
    }
    return ret;
}

public
int _int_to_str_s(char *out, size_t bufsize, int n) {
    char *start;

    // Handle negative numbers.
    if (n < 0) {
        if (!bufsize) return -1;

        *out++ = '-';
        bufsize--;
    }

    // Remember the start of the string...  This will come into play at the end
    start = out;

    do {
        // Handle the current digit.
        int digit;
        if (!bufsize) return -1;
        digit = n % 10;
        if (digit < 0) digit *= -1;
        *out++ = digit + '0';
        bufsize--;
        n /= 10;
    } while (n);

    // Terminate the string.
    if (!bufsize) return -1;
    *out = 0;

    // We wrote the string backwards, i.e. with least significant digits first.
    // Now reverse the string.
    --out;
    while (start < out) {
        char a = *start;
        *start = *out;
        *out = a;
        ++start;
        --out;
    }

    return 0;
}

public
int _int_to_str_u(char *out, size_t bufsize, unsigned int n) {
    char *start;

    // Remember the start of the string...  This will come into play at the end
    start = out;

    do {
        // Handle the current digit.
        int digit;
        if (!bufsize) return -1;
        digit = n % 10;
        if (digit < 0) digit *= -1;
        *out++ = digit + '0';
        bufsize--;
        n /= 10;
    } while (n);

    // Terminate the string.
    if (!bufsize) return -1;
    *out = 0;

    // We wrote the string backwards, i.e. with least significant digits first.
    // Now reverse the string.
    --out;
    while (start < out) {
        char a = *start;
        *start = *out;
        *out = a;
        ++start;
        --out;
    }

    return 0;
}

public
char *_int_to_str_static(unsigned int num, int base) {
    static char Representation[] = "0123456789ABCDEF";
    static char buffer[64] = {0};
    char *ptr;

    ptr = &buffer[63];
    *ptr = '\0';

    do {
        *--ptr = Representation[num % base];
        num /= base;
    } while (num != 0);

    return (ptr);
}

#ifdef __cplusplus
}
#endif
