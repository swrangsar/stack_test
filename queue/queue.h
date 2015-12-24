#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "types.h"

typedef struct _Queue Queue;

Queue queue_new(DestroyFunc);
int enqueue(Queue *, void *);
int queue_is_empty(Queue *);
void *dequeue(Queue *);
void queue_destroy(Queue *);

#endif
