#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define log_err(M)	{perror("error: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: " M "\n"); goto error;}

typedef struct _Node Node;

struct _Node {
	Node *next;
	void *data;
};

struct _Stack {
	Node *head;
};


Stack* stack_new(void)
{
	Stack *stack;
	
	stack = (Stack *)malloc(sizeof(Stack));
	if (!stack)
		log_err("stack_new");

	stack->head = NULL;

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

	node = (Node *)malloc(sizeof(Node));
	if (!node)
		log_err("stack_push: node allocate failed!");

	node->data = data;
	node->next = stack->head;
	stack->head = node;

out:
	return 0;
error:
	return -1;
}

void stack_destroy(Stack* stack)
{
	if (!stack)
		return;

	stack->head = NULL;
	free((void *)stack);
}
