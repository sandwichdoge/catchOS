#ifndef INCLUDE_LIST_H
#define INCLUDE_LIST_H

struct list_head {
    struct list_head *next;
    void *data;
};

struct list_head *list_create(void *data, int data_size);
struct list_head *list_insert_before(struct list_head *list, void *data, int data_size);
struct list_head *list_insert_after(struct list_head *node, void *data, int data_size);
struct list_head *list_insert_back(struct list_head *node, void *data, int data_size);
int list_remove(struct list_head *list, struct list_head *node);
int list_foreach(struct list_head *node, int (*func)(void *));
int list_free(struct list_head **list);
struct list_head *list_get_tail_fast();
struct list_head *list_get_tail(struct list_head *list);
struct list_head *list_find(struct list_head *node, int (*func)(void *, void *), void *data);

#endif
