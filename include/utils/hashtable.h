#ifndef INCLUDE_HASHTABLE_H
#define INCLUDE_HASHTABLE_H
#include "utils/list.h"
#include "stdint.h"
#include "stddef.h"

struct hashtable {
    struct list_head** nodes;
    size_t size;
    size_t (*hash)(char*, size_t);
};

// Init a hash table, leave hashfunc NULL to use default hasher algorithm (FNV).
int hashtable_init(struct hashtable* ht, size_t (*hashfunc)(char*, size_t));

#endif
