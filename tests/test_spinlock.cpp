#include "gtest/gtest.h"
extern "C" {
#include "../include/utils/spinlock.h"
#include "../include/utils/rwlock.h"
}
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

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

struct rwlock rwlock = {0};
int rw_counter = 0;
void* rw_read(void* unused) {
    rwlock_read_acquire(&rwlock);
    printf("read counter[%d]\n", rw_counter);
    rwlock_read_release(&rwlock);
}

void* rw_write(void* unused) {
    rwlock_write_acquire(&rwlock);
    rw_counter++;
    rwlock_write_release(&rwlock);
}

TEST(rwlock, basic) {
    rwlock_init(&rwlock);
    pthread_t pool_r1[32] = {0};
    pthread_t pool_r2[32] = {0};
    pthread_t pool_w[32] = {0};
    for (int i = 0; i < 32; ++i) {
        pthread_create(&pool_w[i], NULL, rw_write, NULL);
        pthread_create(&pool_r1[i], NULL, rw_read, NULL);
        pthread_create(&pool_r2[i], NULL, rw_read, NULL);
    }

    for (int i = 0; i < 32; ++i) {
        pthread_join(pool_r1[i], NULL);
        pthread_join(pool_r2[i], NULL);
        pthread_join(pool_w[i], NULL);
    }
    EXPECT_EQ(rw_counter, 32);
}
