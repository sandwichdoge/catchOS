#include "gtest/gtest.h"
#include "utils/bitmap.h"

TEST(Bitmap, bitset) {
    unsigned char bitmap[32] = {0};
    bitmap_set_bit(bitmap, 10);
    int ninth = bitmap_get_bit(bitmap, 9);
    int tenth = bitmap_get_bit(bitmap, 10);
    int eleventh = bitmap_get_bit(bitmap, 11);

    EXPECT_EQ(ninth, 0);
    EXPECT_EQ(tenth, 1);
    EXPECT_EQ(eleventh, 0);
}

TEST(Bitmap, bitclear) {
    unsigned char bitmap[32] = {0};
    bitmap_set_bit(bitmap, 10);
    int tenth = bitmap_get_bit(bitmap, 10);
    EXPECT_EQ(tenth, 1);

    bitmap_clear_bit(bitmap, 10);
    tenth = bitmap_get_bit(bitmap, 10);
    EXPECT_EQ(tenth, 0);
}

TEST(Bitmap, bitset_multi) {
    unsigned char bitmap[32] = {0};
    bitmap_set_bits(bitmap, 35, 7);
    for (int i = 35; i < 35 + 7; ++i) {
        int bit = bitmap_get_bit(bitmap, i);
        EXPECT_EQ(bit, 1);
    }

    int tmp = bitmap_get_bit(bitmap, 50);
    EXPECT_EQ(tmp, 0);
}

TEST(Bitmap, bittoggle) {
    unsigned char bitmap[32] = {0};
    bitmap_toggle_bit(bitmap, 79);
    int bit = bitmap_get_bit(bitmap, 79);
    EXPECT_EQ(bit, 1);

    bitmap_toggle_bit(bitmap, 79);
    bit = bitmap_get_bit(bitmap, 79);
    EXPECT_EQ(bit, 0);
}
