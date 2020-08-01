#include "utils/hashtable.h"
#include "mmu.h"
#include "builddef.h"
#include "utils/debug.h"

int hashtable_init(struct hashtable* ht, size_t (*hashfunc)(char*, size_t), size_t size) {
    ht->hash = hashfunc;
    ht->size = size;
    ht->nodes = mmu_mmap(size * sizeof(*ht->nodes));
    if (!ht->nodes) {
        return -1;
    } else {
        return 0;
    }
}

void hashtable_uninit(struct hashtable* ht) {
    mmu_munmap(ht->nodes);
}

void hashtable_insert(struct hashtable* ht, char* key, void* data, size_t data_size) {
    size_t index = ht->hash(key, ht->size);
    if (!ht->nodes[index]) {
        ht->nodes[index] = list_create(data, data_size);
    } else {
        list_insert_back(ht->nodes[index], data, data_size);
    }
}

int hashtable_get(struct hashtable* ht, char* key, void* out, size_t data_size) {
    size_t index = ht->hash(key, ht->size);
    if (ht->nodes[index]) {
        struct list_head* real = list_find(ht->nodes[index], _strcmp, key);
        if (real) {
            _memcpy(out, real->data, data_size);
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}
