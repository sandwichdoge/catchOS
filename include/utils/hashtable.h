#ifndef INCLUDE_HASHTABLE_H
#define INCLUDE_HASHTABLE_H
#include "utils/list.h"
#include "stdint.h"
#include "stddef.h"

#define HT_MAX_KEY_LEN 64

struct ht_pair {
    struct list_head head;
    char key[HT_MAX_KEY_LEN];
};

struct hashtable {
    struct ht_pair** nodes;
    size_t size;
    size_t (*hash)(char*, size_t);
};

// Init a hash table, leave hashfunc NULL to use default hasher algorithm (FNV).
int hashtable_init(struct hashtable* ht, size_t (*hashfunc)(char*, size_t), size_t size);
void hashtable_uninit(struct hashtable* ht);
void hashtable_insert(struct hashtable* ht, char* key, void* data, size_t data_size);
int hashtable_get(struct hashtable* ht, char* key, void* out, size_t data_size);
#endif
