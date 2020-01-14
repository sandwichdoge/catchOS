#include <assert.h>
#include <stdio.h>

void test_algo_longest_consecutive_zeroes_in_byte() {
    unsigned char n = 0b01001000;

    int j = 0, longest_len = 0, longest_pos = 0, cur_len = 0, last_one = 0;
    for (j = 0; j < 8; j++) {
        if ((n & (1 << (7 - j))) == 0) {
            cur_len++;
            if (j == 7) {
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
            last_one = j;
        }
    }

    printf("Longest:[%d][%d]\n", longest_pos, longest_len);

    assert(longest_pos == 5);
    assert(longest_len == 3);
}


void test_algo_longest_consecutive_zeroes_in_byte2() {
    unsigned char n = 0b01111111;

    int j = 0, longest_len = 0, longest_pos = 0, cur_len = 0, last_one = -1;
    for (j = 0; j < 8; j++) {
        if ((n & (1 << (7 - j))) == 0) {
            cur_len++;
            if (j == 7) {
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
            last_one = j;
        }
    }

    printf("Longest:[%d][%d]\n", longest_pos, longest_len);

    assert(longest_pos == 0);
    assert(longest_len == 1);
}

void test_algo_longest_consecutive_zeroes_in_byte3() {
    unsigned char n = 0b11000000;

    int j = 0, longest_len = 0, longest_pos = 0, cur_len = 0, last_one = -1;
    for (j = 0; j < 8; j++) {
        if ((n & (1 << (7 - j))) == 0) {
            cur_len++;
            if (j == 7) {
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
            last_one = j;
        }
    }

    printf("Longest:[%d][%d]\n", longest_pos, longest_len);

    assert(longest_pos == 2);
    assert(longest_len == 6);
}

void test_algo_most_fit_byte() {
    unsigned char n = 0b10010001;

    int pages = 2;
    int j = 0, shortest_fit_len = 8, shortest_fit_pos = 0, cur_len = 0, last_one = -1;
    for (j = 0; j < 8; j++) {
        if ((n & (1 << (7 - j))) == 0) {
            cur_len++;
            if (j == 7) {
                if (shortest_fit_len > cur_len && cur_len >= pages) {
                    shortest_fit_len = cur_len;
                    shortest_fit_pos = last_one + 1;
                }
            }
        } else {
            if (shortest_fit_len > cur_len && cur_len >= pages) {
                shortest_fit_len = cur_len;
                shortest_fit_pos = last_one + 1;
            }
            cur_len = 0;
            last_one = j;
        }
    }

    printf("Shortest fit:[%d][%d]\n", shortest_fit_pos, shortest_fit_len);
    assert(shortest_fit_len == 2);
    assert(shortest_fit_pos == 1);
}

void test_algo_most_fit_byte2() {
    unsigned char n = 0b00000011;

    int pages = 1;
    int j = 0, best_fit_len = 9, best_fit_pos = -1, cur_len = 0, last_one = -1;
    for (j = 0; j < 8; j++) {
        if ((n & (1 << j)) == 0) {
            cur_len++;
            if (j == 7) {
                if (best_fit_len > cur_len && cur_len >= (int)pages) {
                    best_fit_len = cur_len;
                    best_fit_pos = last_one + 1;
                }
            }
        } else {
            if (best_fit_len > cur_len && cur_len >= (int)pages) {
                best_fit_len = cur_len;
                best_fit_pos = last_one + 1;
            }
            cur_len = 0;
            last_one = j;
        }
    }

    printf("Shortest fit:[%d][%d]\n", best_fit_pos, best_fit_len);
    assert(best_fit_len == 6);
    assert(best_fit_pos == 2);
}

void test_algo_most_fit_byte3() {
    unsigned char n = 0b01010101;

    int pages = 2;
    int j = 0, shortest_fit_len = 9, shortest_fit_pos = -1, cur_len = 0, last_one = -1;
    for (j = 0; j < 8; j++) {
        if ((n & (1 << (7 - j))) == 0) {
            cur_len++;
            if (j == 7) {
                if (shortest_fit_len > cur_len && cur_len >= pages) {
                    shortest_fit_len = cur_len;
                    shortest_fit_pos = last_one + 1;
                }
            }
        } else {
            if (shortest_fit_len > cur_len && cur_len >= pages) {
                shortest_fit_len = cur_len;
                shortest_fit_pos = last_one + 1;
            }
            cur_len = 0;
            last_one = j;
        }
    }

    printf("Shortest fit:[%d][%d]\n", shortest_fit_pos, shortest_fit_len);
    assert(shortest_fit_len == 9);
    assert(shortest_fit_pos == -1);
}

static unsigned char pageframe_bitmap[4096];
static void pageframe_alloc_set_page(unsigned int page_no) {
    unsigned int byte_no = page_no / 8;
    unsigned int carry_bit = page_no % 8;
    pageframe_bitmap[byte_no] = (1 << carry_bit);
}
void test_algo_set_page() {
    pageframe_alloc_set_page(3);
    printf("bitmap[0] %d\n", pageframe_bitmap[0]);
    assert(pageframe_bitmap[0] == 8);
}