#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>


#define log_err(M)	{perror("error: stack: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: stack: " M "\n"); goto error;}


typedef struct _Node Node;

struct _Node {
	Node *next;
	void *data;
};

struct _Stack {
	Node *head;
	DestroyFunc destroy_func;
};


static void stack_clear(Stack *stack);


Stack *stack_new(DestroyFunc destroy_func)
{
	Stack *stack;
	
	if (!(stack = malloc(sizeof(*stack))))
		log_err("stack_new");

	stack->head = NULL;
	stack->destroy_func = destroy_func;
error:
	return stack; 
}


int stack_push(Stack *stack, void *data)
{
	Node *node;

	if (!stack)
		log_msg("stack_push: null stack");

	if (!(node = malloc(sizeof(*node))))
		log_err("stack_push: node allocate failed!");

	node->data = data;
	node->next = stack->head;
	stack->head = node;

	return 0;
error:
	return -1;
}

int stack_is_empty(Stack *stack)
{
	if (!stack)
		log_msg("stack_is_empty: null stack!");

	return !stack->head;
error:
	return 1;
}

void *stack_pop(Stack *stack)
{
	Node *node;
	void *data;

	if (!stack)
		log_msg("stack_pop: null stack!");
	if (!(node = stack->head))
		return NULL;

	data = node->data;
	stack->head = node->next;
	free(node);
	return data;
error:
	return NULL;
}

static void stack_clear(Stack *stack)
{
	if (!stack)
		return;
	
	if (stack->destroy_func) {
		while (!stack_is_empty(stack))
			stack->destroy_func(stack_pop(stack));
	} else {
		while (!stack_is_empty(stack))
			stack_pop(stack);
	}
}

void stack_destroy(Stack* stack)
{
	if (!stack)
		return;

	stack_clear(stack);
	free(stack);
}
