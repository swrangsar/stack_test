#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>

#include "types.h"
#include "stack.h"


struct Queue {
	struct node *head;
	struct node *tail;
	DestroyFunc destroy_func;
};

bool queue_init(DestroyFunc destroy_func, struct Queue *queue);
int enqueue(struct Queue *, void *);
bool is_queue_empty(struct Queue *queue);
void *dequeue(struct Queue *);
void queue_destroy(struct Queue *);

#endif  // QUEUE_H_
