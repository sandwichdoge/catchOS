#include "utils/string.h"

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

void _memset_s(char *dst, char c, unsigned int len) {
    while (len--) {
        *(dst++) = c;
    }
}

void _memset_u(unsigned char *dst, char c, unsigned int len) {
    while (len--) {
        *(dst++) = c;
    }
}

void _memcpy_s(char *src, char *dst, unsigned int len) {
    while (len--) {
        *(dst++) = *(src++);
    }
}

void _memcpy_u(unsigned char *src, char *dst, unsigned int len) {
    while (len--) {
        *(dst++) = *(src++);
    }
}

int _int_to_str_s(char *out, size_t bufsize, int n)
{
    char *start;

    // Handle negative numbers.
    if (n < 0) {
        if (!bufsize)
            return -1;

        *out++ = '-';
        bufsize--;
    }

    // Remember the start of the string...  This will come into play at the end
    start = out;

    do {
        // Handle the current digit.
        int digit;
        if (!bufsize)
            return -1;
        digit = n % 10;
        if (digit < 0)
            digit *= -1;
        *out++ = digit + '0';
        bufsize--;
        n /= 10;
    } while (n);

    // Terminate the string.
    if (!bufsize)
        return -1;
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

int _int_to_str_u(char *out, size_t bufsize, unsigned int n)
{
    char *start;

    // Remember the start of the string...  This will come into play at the end
    start = out;

    do {
        // Handle the current digit.
        int digit;
        if (!bufsize)
            return -1;
        digit = n % 10;
        if (digit < 0)
            digit *= -1;
        *out++ = digit + '0';
        bufsize--;
        n /= 10;
    } while (n);

    // Terminate the string.
    if (!bufsize)
        return -1;
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