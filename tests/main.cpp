/* These tests should run on host computer with full glibc*/

#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(GROUP1, SANITY) { EXPECT_EQ(2, 2); }