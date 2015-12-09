#ifndef _STACK_H_
#define _STACK_H_

typedef struct _Stack Stack;
typedef void (*DestroyFunc) (void *);

Stack* stack_new(DestroyFunc);
int stack_push(Stack*, void*);
void *stack_pop(Stack*);
void stack_destroy(Stack*);

#endif
