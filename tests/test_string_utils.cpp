#include <cstring>

#include "../include/utils/string.h"
#include "gtest/gtest.h"

TEST(StringUtils, memset) {
    char s[] = "hello, world";
    _memset_s(s, 'z', 5);
    EXPECT_TRUE(0 == std::memcmp(s, "zzzzz, world", sizeof(s)));
}

TEST(StringUtils, memcpy) {
    char s[] = "hello, world";
    _memcpy_s(s, "adios", 5);
    EXPECT_TRUE(0 == std::memcmp(s, "adios, world", sizeof(s)));
}

TEST(StringUtils, memcpy2) {
    char s[] = "hello, world";
    char *prev = s;
    _memcpy_s(s, "adios", 5);
    EXPECT_EQ(prev, s);
}

TEST(StringUtils, memcpy_void) {
    char *s1 = "abcdef";
    char s2[12] = {0};
    _memcpy_s((void *)s2, (void *)s1, 6);
    printf("%s\n", s2);
    EXPECT_EQ(_strncmp(s1, s2, 6), 0);
}

TEST(StringUtils, strlen) {
    char *s = "hello\0";
    unsigned int len = _strlen(s);
    EXPECT_EQ(len, 5);
}

TEST(StringUtils, strcpy) {
    char *src = "obiwan";
    char dest[16] = {0};
    _strcpy_s(dest, src);
    EXPECT_TRUE(0 == std::strcmp(dest, "obiwan"));
}

TEST(StringUtils, strncpy) {
    char *src = "obiwan";
    char dest[16] = {0};
    _strncpy_s(dest, src, 3);
    EXPECT_TRUE(0 == std::strcmp(dest, "obi"));
}

TEST(StringUtils, strncmp) {
    char *s1 = "abc\0";
    char *s2 = "abc\0";

    int r = _strncmp(s1, s2, 3);
    EXPECT_EQ(r, 0);

    char *s3 = "xyz";
    char *s4 = "xyzz";
    r = _strncmp(s3, s4, 4);
    EXPECT_LT(r, 0);

    char *s5 = "xyz";
    char *s6 = "xyZ";
    r = _strncmp(s5, s6, 3);
    EXPECT_GT('z' - 'Z', 0);
    EXPECT_GT(r, 0);
}