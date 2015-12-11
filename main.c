#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int compare(const void *a, const void *b)
{
	int num_a = (int)a;
	int num_b = (int)b;
	
	if (num_a == num_b)
		return 0;
	
	return (num_a > num_b)?1:-1;
}

int main(const int argc, const char **argv)
{
	RBTree *num_tree;
	int my_num = 719203;
	int i;

	num_tree = rbtree_new(compare, NULL);

	if (!num_tree)
		return -1;

	for (i = 0; i < 100; ++i) {
		my_num = (my_num ^ (my_num >> 3));
		my_num += (my_num << 5);
		my_num %= 100;
		printf("ins i=%d, my_num=%d\n", i, my_num);
	
		if (rbtree_insert(num_tree, (void *)my_num))
			fprintf(stderr, "error: rbtree_insert: num_tree: data=%d\n", my_num);
	}

	for (i = 0; i < 100; ++i) {
		my_num = (my_num ^ (my_num >> 7));
		my_num += (my_num << 3);
		my_num %= 100;
		printf("del i=%d, my_num=%d\n", i, my_num);
	
		if (rbtree_remove(num_tree, (void *)my_num))
			fprintf(stderr, "error: rbtree_remove: num_tree: data=%d\n", my_num);
	}

	
	rbtree_destroy(num_tree);
	num_tree = NULL;
	
	return 0;
}
