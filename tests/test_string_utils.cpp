#include "../include/utils/string.h"
#include "gtest/gtest.h"
#include <cstring>

TEST(StringUtils, memset) 
{
    char s[] = "hello, world";
    _memset_s(s, 'z', 5);
    EXPECT_TRUE(0 == std::memcmp(s, "zzzzz, world", sizeof(s)));
} 


TEST(StringUtils, memcpy) {
    char s[] = "hello, world";
    _memcpy_s("adios", s, 5);
    EXPECT_TRUE(0 == std::memcmp(s, "adios, world", sizeof(s)));
}

TEST(StringUtils, memcpy2) {
    char s[] = "hello, world";
    char *prev = s;
    _memcpy_s("adios", s, 5);
    EXPECT_EQ(prev, s);
}

TEST(StringUtils, strlen) {
    char *s = "hello\0";
    unsigned int len = _strlen(s);
    EXPECT_EQ(len, 5);
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