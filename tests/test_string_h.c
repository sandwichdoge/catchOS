#include "../string.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

int test_memset() {
    char s[] = "hello, world";
    _memset(s, 'z', 5);
    printf("%s\n", s);
    assert(strcmp(s, "zzzzz, world") == 0);

    return 0;
}

int test_memcpy() {
    char s[] = "hello, world";
    _memcpy("adios", s, 5);
    printf("%s\n", s);
    assert(strcmp(s, "adios, world") == 0);

    return 0;
}

int test_strlen() {
    char *s = "hello\0";
    unsigned int len = _strlen(s);
    assert(len == 5);

    return 0;
}

int test_strncmp() {
    char *s1 = "abc\0";
    char *s2 = "abc\0";

    int r = _strncmp(s1, s2, 3);
    assert(r == 0);

    char *s3 = "xyz";
    char *s4 = "xyzz";
    r = _strncmp(s3, s4, 4);
    assert(r < 0);

    char *s5 = "xyz";
    char *s6 = "xyZ";
    r = _strncmp(s5, s6, 3);
    assert('z' - 'Z' > 0);
    assert(r > 0);
}