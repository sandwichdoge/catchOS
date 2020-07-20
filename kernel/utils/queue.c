#include "utils/queue.h"
#include "utils/list.h"
#include "utils/string.h"

void queue_push(struct queue* Q, void* data, int data_size) {
    if (!Q->head) {
        Q->head = list_create(data, data_size);
        Q->tail = Q->head;
        Q->size = 1;
    } else {
        struct list_head* p = list_insert_after(Q->tail, data, data_size);
        Q->tail = p;
        Q->size++;
    }
}

int queue_pop(struct queue* Q, void* data, int data_size) {
    if (!Q->head) {
        return -1;
    } else {
        _memcpy(data, Q->head->data, data_size);
        struct list_head *oldhead = Q->head;
        struct list_head *newhead = Q->head->next;
        Q->head = newhead;
        list_remove(oldhead, oldhead);
        Q->size--;
        return 0;
    }
}

int queue_size(struct queue *Q) {
    return Q->size;
}
