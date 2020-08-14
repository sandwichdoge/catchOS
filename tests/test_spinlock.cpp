#include "gtest/gtest.h"
extern "C" {
#include "../include/utils/spinlock.h"
}
#include <pthread.h>
#include <stdio.h>

struct spinlock lock = {0};
int counter = 0;

void* count(void* unused) {
    spinlock_lock(&lock);
    for (int i = 0; i < 200000; ++i) {
        counter++;
    }
    spinlock_unlock(&lock);
    return NULL;
}

TEST(Spinlock, basic) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, count, NULL);
    pthread_create(&t2, NULL, count, NULL);
    pthread_join(t2, NULL);
    pthread_join(t1, NULL);
    printf("%d\n", counter);
    EXPECT_EQ(counter, 400000);
}
