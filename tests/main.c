/* These tests should run on host computer with full glibc*/

extern int test_memset();
extern int test_memcpy();
extern int test_strlen();

int main() {
    test_memset();
    test_memcpy();
    test_strlen();
    return 0;
}