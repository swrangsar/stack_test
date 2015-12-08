#ifndef _STACK_H_
#define _STACK_H_

typedef struct _Stack Stack;

Stack* stack_new(void);
int stack_push(Stack*, void*);
void stack_destroy(Stack*);

#endif
