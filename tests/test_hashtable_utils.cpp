extern "C" {
#include "../include/utils/hashtable.h"
}
#include "gtest/gtest.h"

TEST(Hashtable, init) {
    struct hashtable ht;
    hashtable_init(&ht, NULL, 64);
    hashtable_uninit(&ht);
}

TEST(Hashtable, insert) {
    struct hashtable ht;
    hashtable_init(&ht, NULL, 64);
    int n = 123;
    hashtable_insert(&ht, "one", &n, sizeof(n));
    int n2 = 0;
    hashtable_get(&ht, "one", &n2, sizeof(n2));
    hashtable_uninit(&ht);
    EXPECT_EQ(n, n2);
}
