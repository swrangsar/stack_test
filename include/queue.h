#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

typedef struct Queue_ Queue;

Queue *queue_new(DestroyFunc);
int enqueue(Queue *, void *);
int queue_is_empty(Queue *);
void *dequeue(Queue *);
void queue_destroy(Queue *);

#endif
