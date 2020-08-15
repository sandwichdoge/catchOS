#include "gtest/gtest.h"
extern "C" {
#include "../include/utils/spinlock.h"
#include "../include/utils/rwlock.h"
}
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

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
    EXPECT_EQ(counter, 100);
}

struct rwlock rwlock = {0};
int rw_counter = 0;
void* rw_read(void* unused) {
    rwlock_read_acquire(&rwlock);
    for (int i = 0; i < 20000; ++i) {
        static size_t prev;
        if (prev != pthread_self()) {
            printf("rw[%lu][%d]\n", pthread_self(), rw_counter);
        }
        prev = pthread_self();
    }
    rwlock_read_release(&rwlock);
    return NULL;
}

void* rw_write(void* unused) {
    rwlock_write_acquire(&rwlock);
    rw_counter++;
    rwlock_write_release(&rwlock);
    return NULL;
}

TEST(rwlock, basic) {
    clock_t before = clock();
    rwlock_init(&rwlock);
    pthread_t pool_r1[4] = {0};
    pthread_t pool_r2[4] = {0};
    pthread_t pool_w[4] = {0};
    for (int i = 0; i < sizeof(pool_w) / sizeof(*pool_w); ++i) {
        //pthread_create(&pool_w[i], NULL, rw_write, NULL);
    }
    for (int i = 0; i < sizeof(pool_r1) / sizeof(*pool_r1); ++i) {
        pthread_create(&pool_r1[i], NULL, rw_read, NULL);
        pthread_create(&pool_r2[i], NULL, rw_read, NULL);
    }

    for (int i = 0; i < sizeof(pool_w) / sizeof(*pool_w); ++i) {
        pthread_join(pool_w[i], NULL);
    }
    for (int i = 0; i < sizeof(pool_r1) / sizeof(*pool_r1); i++) {
        pthread_join(pool_r1[i], NULL);
        pthread_join(pool_r2[i], NULL);
    }

    clock_t after = clock();
    auto diff = after - before;
    printf("RW lock took %lu\n", diff );
    EXPECT_EQ(rw_counter, 4);
}

struct spinlock basiclock = {0};
int basic_counter = 0;
void* basic_write(void* unused) {
    spinlock_lock(&basiclock);
    basic_counter++;
    spinlock_unlock(&basiclock);
    return NULL;
}

void* basic_read(void* unused) {
    spinlock_lock(&basiclock);
    for (int i = 0; i < 20000; ++i) {
        static size_t prev;
        if (prev != pthread_self()) {
            printf("basic[%lu][%d]\n", pthread_self(), rw_counter);
        }
        prev = pthread_self();
    }
    spinlock_unlock(&basiclock);
    return NULL;
}

TEST(spinlock, versus_rwlock) {
    clock_t before = clock();
    pthread_t pool_r1[4] = {0};
    pthread_t pool_r2[4] = {0};
    pthread_t pool_w[4] = {0};
    for (int i = 0; i < sizeof(pool_w) / sizeof(*pool_w); ++i) {
        //pthread_create(&pool_w[i], NULL, basic_write, NULL);
    }

    for (int i = 0; i < sizeof(pool_r1) / sizeof(*pool_r1); ++i) {
        pthread_create(&pool_r1[i], NULL, basic_read, NULL);
        pthread_create(&pool_r2[i], NULL, basic_read, NULL);
    }

    for (int i = 0; i < sizeof(pool_w) / sizeof(*pool_w); ++i) {
        pthread_join(pool_w[i], NULL);
    }
    for (int i = 0; i < sizeof(pool_r1) / sizeof(*pool_r1); i++) {
        pthread_join(pool_r1[i], NULL);
        pthread_join(pool_r2[i], NULL);
    }

    clock_t after = clock();
    auto diff = after - before;
    printf("Basic lock took %lu\n", diff );
    EXPECT_EQ(basic_counter, 4);
}
