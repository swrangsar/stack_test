#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define MAX_TIMES	9977
#define BUF_SIZE	128

int compare(const void *a, const void *b)
{
	const char *str_a = (const char *)a;
	const char *str_b = (const char *)b;
	
	return strcmp(str_a, str_b);
}

void destroy_str(void *data)
{
	free(data);
}

int main(const int argc, const char **argv)
{
	RBTree *num_tree;
	int my_num = 719203;
	int i;
	char buf[BUF_SIZE+1];
	char *new_str = NULL;
	size_t len;

	num_tree = rbtree_new(compare, destroy_str);

	if (!num_tree)
		return -1;

	for (i = 0; i < MAX_TIMES; ++i) {
		my_num = (my_num ^ (my_num >> 3));
		my_num += (my_num << 5);
		my_num %= MAX_TIMES;
		
/*		printf("ins %d\n", my_num); */
		sprintf(buf, "%d", my_num);
		len = strlen(buf);
		new_str = malloc(sizeof(char) * (1 + len));
		if (new_str) {
			strcpy(new_str, buf);
			printf("i %s\n", new_str);
		}
	
		if (rbtree_insert(num_tree, (void *)new_str) && new_str)
			fprintf(stderr, "error: rbtree_insert: num_tree: data=%s\n", new_str);
		new_str = NULL;
	}

	for (i = 0; i < MAX_TIMES; ++i) {
		my_num = (my_num ^ (my_num >> 7));
		my_num += (my_num << 3);
		my_num %= MAX_TIMES;
/*		printf("del %d\n", my_num); */
		sprintf(buf, "%d", my_num);
		printf("d %s\n", buf);
	
		if (rbtree_remove(num_tree, (void *)buf))
			fprintf(stderr, "error: rbtree_remove: num_tree: data=%s\n", buf);
	}

	
	rbtree_destroy(num_tree);
	num_tree = NULL;
	
	return 0;
}
