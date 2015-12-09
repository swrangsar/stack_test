#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char **argv)
{
	Stack *stack;
	int my_num = 719203;

	stack = stack_new(NULL);
	if (!stack)
		return -1;
	
	printf("test: push: %d\n", my_num);
	stack_push(stack, (void *)my_num);
	
	printf("test: pop: %d\n", (int)stack_pop(stack));

	stack_destroy(stack);
	
	return 0;
}
