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

TEST(Hashtable, collision) {
    struct hashtable ht;
    hashtable_init(&ht, NULL, 1);
    int n = 123, m = 456;
    hashtable_insert(&ht, "one", &n, sizeof(n));
    hashtable_insert(&ht, "two", &m, sizeof(m));
    int n2 = 0, m2 = 0;
    hashtable_get(&ht, "one", &n2, sizeof(n2));
    hashtable_get(&ht, "two", &m2, sizeof(m2));
    hashtable_uninit(&ht);
    EXPECT_EQ(n, n2);
    EXPECT_EQ(m, m2);
}