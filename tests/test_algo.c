#include <assert.h>
#include <stdio.h>

void test_algo_longest_consecutive_zeroes_in_byte() {
    unsigned char n = 0b01001000;

    int i = 0, longest_len = 0, longest_pos = 0, cur_len = 0, last_one = 0;
    for (i = 0; i < 8; i++) {
        if ((n & (1 << (7 - i))) == 0) {
            cur_len++;
            if (i == 7) {
                if (longest_len < cur_len) {
                    longest_len = cur_len;
                    longest_pos = last_one + 1;
                }
            }
        } else {
            if (longest_len < cur_len) {
                longest_len = cur_len;
                longest_pos = last_one + 1;
            }
            cur_len = 0;
            last_one = i;
        }
    }

    printf("Longest:[%d][%d]\n", longest_pos, longest_len);

    assert(longest_pos == 5);
    assert(longest_len == 3);
}


void test_algo_longest_consecutive_zeroes_in_byte2() {
    unsigned char n = 0b01111111;

    int i = 0, longest_len = 0, longest_pos = 0, cur_len = 0, last_one = -1;
    for (i = 0; i < 8; i++) {
        if ((n & (1 << (7 - i))) == 0) {
            cur_len++;
            if (i == 7) {
                if (longest_len < cur_len) {
                    longest_len = cur_len;
                    longest_pos = last_one + 1;
                }
            }
        } else {
            if (longest_len < cur_len) {
                longest_len = cur_len;
                longest_pos = last_one + 1;
            }
            cur_len = 0;
            last_one = i;
        }
    }

    printf("Longest:[%d][%d]\n", longest_pos, longest_len);

    assert(longest_pos == 0);
    assert(longest_len == 1);
}

void test_algo_longest_consecutive_zeroes_in_byte3() {
    unsigned char n = 0b10000000;

    int i = 0, longest_len = 0, longest_pos = 0, cur_len = 0, last_one = -1;
    for (i = 0; i < 8; i++) {
        if ((n & (1 << (7 - i))) == 0) {
            cur_len++;
            if (i == 7) {
                if (longest_len < cur_len) {
                    longest_len = cur_len;
                    longest_pos = last_one + 1;
                }
            }
        } else {
            if (longest_len < cur_len) {
                longest_len = cur_len;
                longest_pos = last_one + 1;
            }
            cur_len = 0;
            last_one = i;
        }
    }

    printf("Longest:[%d][%d]\n", longest_pos, longest_len);

    assert(longest_pos == 1);
    assert(longest_len == 7);
}