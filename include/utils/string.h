#ifndef INCLUDE_STRING_H
#define INCLUDE_STRING_H
unsigned int _strlen(char *s);
int _strncmp(char *s1, char *s2, unsigned int len);

void _memset(char *dst, char c, unsigned int len);
void _memset_s(char *dst, char c, unsigned int len);
void _memset_u(unsigned char *dst, char c, unsigned int len);

void _memcpy(char *src, char *dst, unsigned int len);
void _memcpy_s(char *src, char *dst, unsigned int len);
void _memcpy_u(unsigned char *src, char *dst, unsigned int len);

#define _memcpy(src, dst, len) _Generic((src), char*: _memcpy_s, \
                              default: _memcpy_s, \
                              unsigned char*: _memcpy_u)(src, dst, len)

#define _memset(dst, c, len) _Generic((dst), char*: _memset_s, \
                              default: _memset_s, \
                              unsigned char*: _memset_u)(dst, c, len)
#endif