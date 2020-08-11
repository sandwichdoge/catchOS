#include "utils/list.h"

#include "mmu.h"
#include "utils/string.h"

struct list_head *list_create_noalloc(void *data, int data_size, struct list_head *newnode) {
    void *tmp;
    if (!(tmp = mmu_mmap(data_size))) {
        return NULL;
    }
    _memcpy(tmp, data, data_size);
    newnode->data = tmp;
    newnode->next = NULL;
    return newnode;
}

struct list_head *list_create(void *data, int data_size) {
    void *tmp;
    struct list_head *node;
    if (!(node = mmu_mmap(sizeof(struct list_head)))) {
        return NULL;
    }
    if (!(tmp = mmu_mmap(data_size))) {
        return NULL;
    }
    _memcpy(tmp, data, data_size);
    node->data = tmp;
    node->next = NULL;
    return node;
}

struct list_head *list_insert_after(struct list_head *node, void *data, int data_size) {
    if (node == NULL) return NULL;
    struct list_head *newnode;
    newnode = list_create(data, data_size);
    newnode->next = node->next;
    if (node) node->next = newnode;
    return newnode;
}

struct list_head *list_insert_after_noalloc(struct list_head *node, void *data, int data_size, struct list_head *newnode) {
    if (node == NULL) return NULL;
    list_create_noalloc(data, data_size, newnode);
    newnode->next = node->next;
    if (node) node->next = newnode;
    return newnode;
}

struct list_head *list_insert_back_noalloc(struct list_head *node, void *data, int data_size, struct list_head *newnode) {
    struct list_head *tail = list_get_tail(node);
    return list_insert_after_noalloc(tail, data, data_size, newnode);
}

struct list_head *list_insert_back(struct list_head *node, void *data, int data_size) {
    struct list_head *tail = list_get_tail(node);
    return list_insert_after(tail, data, data_size);
}

struct list_head *list_insert_before(struct list_head *list, void *data, int data_size) {
    struct list_head *newnode;
    newnode = list_create(data, data_size);
    newnode->next = list;
    return newnode;
}

int list_remove(struct list_head *list, struct list_head *node) {
    while (list->next && list->next != node) {
        list = list->next;
    }
    if (list->next) {
        list->next = node->next;
        mmu_munmap(node->data);
        mmu_munmap(node);
        return 0;
    } else {
        return -1;
    }
}

int list_foreach(struct list_head *node, int (*func)(void *)) {
    while (node) {
        if (func(node->data) != 0) return -1;
        node = node->next;
    }
    return 0;
}

int list_free(struct list_head **list) {
    struct list_head *node = *list;
    struct list_head *next_node;
    while (node) {
        next_node = node->next;
        if (node->data != NULL) mmu_munmap(node->data);
        mmu_munmap(node);
        node = next_node;
    }
    *list = NULL;
    return 0;
}

struct list_head *list_get_tail(struct list_head *list) {
    struct list_head *node = list;
    struct list_head *head = NULL;

    while (node) {
        head = node;
        node = node->next;
    }
    return head;
}
struct list_head *list_find(struct list_head *node, int (*func)(void *, void *), void *data) {
    while (node) {
        if (func(node->data, data) == 0) return node;
        node = node->next;
    }
    return NULL;
}
