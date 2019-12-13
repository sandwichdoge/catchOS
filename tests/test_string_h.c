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