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

TEST(Bitmap, bsf) {
    unsigned char bitmap[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
    unsigned int bitno = bitmap_find_first_set(bitmap, 8);
    printf("BSF bitno: %d\n", bitno);
    EXPECT_EQ(bitno, 40);

    unsigned char bitmap2[] = {0x00, 0x00, 0x00, 0x00};
    unsigned int bitno2 = bitmap_find_first_set(bitmap2, 4);
    printf("BSF bitno2: %d\n", bitno2);
    EXPECT_EQ(bitno2, -1);

    unsigned char bitmap3[] = {0x02, 0x00, 0x00, 0x00};
    unsigned int bitno3 = bitmap_find_first_set(bitmap3, 4);
    int bit3 = bitmap_get_bit(bitmap3, 1);
    EXPECT_EQ(bit3, bitno3);
    printf("BSF bitno3: %d\n", bitno3);
}
