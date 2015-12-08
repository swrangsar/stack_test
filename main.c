#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char **argv)
{
	Stack *stack;

	stack = stack_new();
	if (!stack)
		return -1;

	stack_destroy(stack);
	
	return 0;
}
