#ifndef INCLUDE_QUEUE_H
#define INCLUDE_QUEUE_H

enum QUEUE_DATA_TYPE {Q_UINT8, Q_INT8, Q_UINT32, Q_INT32};

union queue_data
{
    unsigned char ui8;
    char i8;
    unsigned int ui32;
    int i32;
};

struct queue_node {
    enum QUEUE_DATA_TYPE item_type;
    union queue_data item;
    struct queue_node *next;
};

struct queue {
    struct queue_node *head;
    struct queue_node *tail;
    unsigned int size;
};

int queue_pop(struct queue_node *out);
void queue_push(struct queue_node *item);

#endif