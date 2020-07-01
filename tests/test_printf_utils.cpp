#include <cstring>

#include "../include/utils/printf.h"
#include "gtest/gtest.h"

TEST(PrintfUtils, snprinf1) {
    int n = 8;
    const char *str = "World";
    char buf[64] = {0};

    _snprintf(buf, 12, "%s%d", str, n);
    EXPECT_TRUE(0 == std::strcmp(buf, "World8"));
}

TEST(PrintfUtils, snprinf2) {
    int d = -123, n = 8;
    char buf[64] = {0};

    _snprintf(buf, 12, "%d%d", n, d);
    std::cout << buf << "\n";
    EXPECT_TRUE(0 == std::strcmp(buf, "8-123"));
}

TEST(PrintfUtils, snprinf3) {
    int d = -123, n = 8;
    const char *str = "World";
    char buf[64] = {0};

    _snprintf(buf, 12, "%d%s%d", d, str, n);
    std::cout << buf << "\n";
    EXPECT_TRUE(0 == std::strcmp(buf, "-123World8"));
}
