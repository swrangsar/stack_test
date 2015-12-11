#ifndef _STACK_H_
#define _STACK_H_

#include "types.h"

typedef struct _Stack Stack;

Stack* stack_new(DestroyFunc);
int stack_push(Stack*, void*);
void *stack_pop(Stack*);
void stack_destroy(Stack*);

#endif
