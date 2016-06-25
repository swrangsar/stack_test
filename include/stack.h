#ifndef STACK_H_
#define STACK_H_

#include "types.h"

typedef struct Stack_ Stack;

Stack *stack_new(DestroyFunc);
int stack_push(Stack*, void*);
int stack_is_empty(Stack *);
void *stack_pop(Stack *);
void stack_destroy(Stack *);

#endif
