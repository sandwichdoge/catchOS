#include "utils/hashtable.h"
#include "builddef.h"
#include "mmu.h"
#include "utils/string.h"

private
size_t hashFNV(char* str, size_t max) {
#define FNV_PRIME 0x811C9DC5;
    size_t hash = 0;
    size_t i = 0;

    for (i = 0; str[i]; ++i) {
        hash *= FNV_PRIME;
        hash ^= ((uint8_t)str[i]);
    }
    return hash % max;
}

int hashtable_init(struct hashtable* ht, size_t (*hashfunc)(char*, size_t), size_t size) {
    if (hashfunc) {
        ht->hash = hashfunc;
    } else {
        ht->hash = hashFNV;
    }
    ht->size = size;
    ht->nodes = mmu_mmap(size * sizeof(*ht->nodes));
    _memset(ht->nodes, 0, size * sizeof(*ht->nodes));
    if (!ht->nodes) {
        return -1;
    } else {
        return 0;
    }
}

void hashtable_uninit(struct hashtable* ht) {
    for (size_t i = 0; i < ht->size; ++i) {
        if (ht->nodes[i]) {
            list_free((struct list_head**)&ht->nodes[i]);
        }
    }
    mmu_munmap(ht->nodes);
}

void hashtable_insert(struct hashtable* ht, char* key, void* data, size_t data_size) {
    size_t index = ht->hash(key, ht->size);
    if (!ht->nodes[index]) {  // Nothing at this index yet.
        ht->nodes[index] = mmu_mmap(sizeof(*ht->nodes[index]));
        list_create_noalloc(data, data_size, (struct list_head*)ht->nodes[index]);
        _memset(ht->nodes[index]->key, 0, sizeof(ht->nodes[index]->key));
        _strncpy(ht->nodes[index]->key, key, sizeof(ht->nodes[index]->key));
    } else {
        struct ht_pair* newnode = mmu_mmap(sizeof(*newnode));
        newnode->head.next = NULL;
        list_insert_back_noalloc((struct list_head*)ht->nodes[index], data, data_size, (struct list_head*)newnode);
        _memset(newnode->key, 0, sizeof(newnode->key));
        _strncpy(newnode->key, key, sizeof(newnode->key));
    }
}

int hashtable_get(struct hashtable* ht, char* key, void* out, size_t data_size) {
    size_t index = ht->hash(key, ht->size);
    if (ht->nodes[index]) {
        struct ht_pair* real = ht->nodes[index];
        while (real) {
            if (_strcmp(real->key, key) == 0) {
                break;
            } else {
                real = (struct ht_pair*)real->head.next;
            }
        }
        if (real) {
            _memcpy(out, real->head.data, data_size);
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}
