#include "stack.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>



#define log_err(M)	{perror("error: stack: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: stack: " M "\n"); goto error;}


static void stack_clear(struct Stack *stack);


struct Stack *stack_new(DestroyFunc destroy_func)
{
	struct Stack *stack;
	
	if (!(stack = malloc(sizeof(*stack))))
		log_err("stack_new");

	stack->head = NULL;
	stack->destroy_func = destroy_func;
error:
	return stack; 
}


int stack_push(struct Stack *stack, void *data)
{
	struct node *node;

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

int stack_is_empty(struct Stack *stack)
{
	if (!stack)
		log_msg("stack_is_empty: null stack!");

	return !stack->head;
error:
	return 1;
}

void *stack_pop(struct Stack *stack)
{
	struct node *node;
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

static void stack_clear(struct Stack *stack)
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

void stack_destroy(struct Stack* stack)
{
	if (!stack)
		return;

	stack_clear(stack);
	free(stack);
}
