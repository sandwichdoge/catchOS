#ifdef __cplusplus
extern "C" {
#endif

#ifndef INCLUDE_STRING_H
#define INCLUDE_STRING_H
#include "stddef.h"
#include "stdint.h"
unsigned int _strlen(char *s);
int _strncmp(char *s1, char *s2, unsigned int len);

void _memset_s(char *dst, char c, unsigned int len);
void _memset_u(unsigned char *dst, char c, unsigned int len);

void _memcpy_s(char *src, char *dst, unsigned int len);
void _memcpy_u(unsigned char *src, char *dst, unsigned int len);

void _strcpy_s(char *src, char *dst);
void _strcpy_u(unsigned char *src, char *dst);

int _int_to_str_s(char *out, size_t bufsize, int n);
int _int_to_str_u(char *out, size_t bufsize, unsigned int n);

#define _strcpy(src, dst) _Generic((src), char*: _strcpy_s, \
                                default: _strcpy_s, \
                                unsigned char*: _strcpy_u)(src, dst)

#define _memcpy(src, dst, len) _Generic((src), char*: _memcpy_s, \
                                default: _memcpy_s, \
                                unsigned char*: _memcpy_u)(src, dst, len)

#define _memset(dst, c, len) _Generic((dst), char*: _memset_s, \
                                default: _memset_s, \
                                unsigned char*: _memset_u)(dst, c, len)

#define _int_to_str(out, bufsize, n) _Generic((n), int: _int_to_str_s, \
                                default: _int_to_str_s, \
                                unsigned int: _int_to_str_u)(out, bufsize, n)
#endif

#ifdef __cplusplus
}
#endif