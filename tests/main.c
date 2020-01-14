/* These tests should run on host computer with full glibc*/

extern int test_memset();
extern int test_memcpy();
extern int test_strlen();
extern int test_strncmp();
extern void test_kmalloc_align();
extern void test_page_to_bit();
extern void test_algo_longest_consecutive_zeroes_in_byte();
extern void test_algo_longest_consecutive_zeroes_in_byte2();
extern void test_algo_longest_consecutive_zeroes_in_byte3();
extern void test_algo_most_fit_byte();
extern void test_algo_most_fit_byte2();
extern void test_algo_most_fit_byte3();
extern void test_algo_set_page();

int main() {
    test_memset();
    test_memcpy();
    test_strlen();
    test_strncmp();

    test_kmalloc_align();
    test_page_to_bit();
    test_algo_longest_consecutive_zeroes_in_byte();
    test_algo_longest_consecutive_zeroes_in_byte2();
    test_algo_longest_consecutive_zeroes_in_byte3();
    test_algo_most_fit_byte();
    test_algo_most_fit_byte2();
    test_algo_most_fit_byte3();
    test_algo_set_page();
    return 0;
}