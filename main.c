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
	
	stack_push(stack, (void *)my_num);

	stack_destroy(stack);
	
	return 0;
}
