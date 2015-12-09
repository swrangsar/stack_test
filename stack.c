#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include "lock.h"

#define log_err(M)	{perror("error: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: " M "\n"); goto error;}

typedef struct _Node Node;

struct _Node {
	Node *next;
	void *data;
};

struct _Stack {
	Node *head;
	unsigned int lock;
};


Stack* stack_new(void)
{
	Stack *stack;
	
	stack = malloc(sizeof(*stack));
	if (!stack)
		log_err("stack_new");

	stack->head = NULL;
	stack->lock = 0;

error:
	return stack; 
}


int stack_push(Stack *stack, void *data)
{
	Node *node;

	if (!stack)
		log_msg("stack_push: null stack");
	
	if (!data)
		goto out;

	node = malloc(sizeof(*node));
	if (!node)
		log_err("stack_push: node allocate failed!");

	node->data = data;

	lock(&stack->lock);

	node->next = stack->head;
	stack->head = node;
	
	unlock(&stack->lock);
out:
	return 0;
error:
	return -1;
}

void stack_destroy(Stack* stack)
{
	if (!stack)
		return;

	lock(&stack->lock);
	if (stack->head)
		free(stack->head);

	stack->head = NULL;

	unlock(&stack->lock);
	free((void *)stack);
}
