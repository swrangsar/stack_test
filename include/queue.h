#ifndef QUEUE_H_
#define QUEUE_H_

#include "types.h"
#include "stack.h"


struct Queue {
	struct node *head;
	struct node *tail;
	DestroyFunc destroy_func;
};

struct Queue *queue_new(DestroyFunc);
int enqueue(struct Queue *, void *);
int queue_is_empty(struct Queue *);
void *dequeue(struct Queue *);
void queue_destroy(struct Queue *);

#endif  // QUEUE_H_
