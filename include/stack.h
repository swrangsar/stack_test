#ifndef STACK_H_
#define STACK_H_

#include "types.h"


struct StackNode {
	struct StackNode *next;
	void *data;
};

struct Stack {
	struct StackNode *head;
	DestroyFunc destroy_func;
};



struct Stack *stack_new(DestroyFunc);
int stack_push(struct Stack*, void*);
int stack_is_empty(struct Stack *);
void *stack_pop(struct Stack *);
void stack_destroy(struct Stack *);

#endif  // STACK_H_
