#ifndef STACK_H
#define STACK_H

#include "types.h"

typedef struct Stack_ Stack;

Stack *stack_new(DestroyFunc);
int stack_push(Stack*, void*);
int stack_is_empty(Stack *);
void *stack_pop(Stack *);
void stack_destroy(Stack *);

#endif
