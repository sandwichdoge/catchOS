#include "utils/bitmap.h"
#include "gtest/gtest.h"

TEST(Bitmap, bitset) {
    char bitmap[32] = {0};
    bitmap_set_bit(bitmap, 10);
    int ninth = bitmap_get_bit(bitmap, 9);
    int tenth = bitmap_get_bit(bitmap, 10);
    int eleventh = bitmap_get_bit(bitmap, 11);

    EXPECT_EQ(ninth, 0);
    EXPECT_EQ(tenth, 1);
    EXPECT_EQ(eleventh, 0);
}

TEST(Bitmap, bitclear) {
    char bitmap[32] = {0};
    bitmap_set_bit(bitmap, 10);
    int tenth = bitmap_get_bit(bitmap, 10);
    EXPECT_EQ(tenth, 1);

    bitmap_clear_bit(bitmap, 10);
    tenth = bitmap_get_bit(bitmap, 10);
    EXPECT_EQ(tenth, 0);
}
