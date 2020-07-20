#ifndef INCLUDE_QUEUE_H
#define INCLUDE_QUEUE_H

#include "utils/list.h"

struct queue {
    struct list_head* head;
    struct list_head* tail;
    int size;
};

// Push a copy of data into queue.
void queue_push(struct queue* Q, void* data, int data_size);

// Return a copy of queue's tail, free tail. Return 0 on success, -1 if queue empty.
int queue_pop(struct queue* Q, void* data, int data_size);

int queue_size(struct queue* Q);

#endif
