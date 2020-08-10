#ifdef __cplusplus
extern "C" {
#endif
#include "utils/string.h"
#include "builddef.h"

public
size_t _strlen(const char *s) {
    size_t ret = 0;
    while (*(s++)) {
        ret++;
    }
    return ret;
}

public
int _strncmp(const char *cs, const char *ct, size_t count) {
	unsigned char c1, c2;
	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}

public
int _strcmp(const char *cs, const char *ct) {
    unsigned char c1, c2;

    while (1) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            break;
    }
    return 0;
}

public
void _memset(void *dst, char c, unsigned int len) {
    char *xs = dst;
    while (len--) {
        *(xs++) = c;
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
char *_strnstr(const char *s1, const char *s2, size_t len) {
    size_t l2;

    l2 = _strlen(s2);
    if (!l2) return (char *)s1;
    while (len >= l2) {
        len--;
        if (!_strncmp(s1, s2, l2)) return (char *)s1;
        s1++;
    }
    return NULL;
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
