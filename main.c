#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>

#define MAX_TIMES	971
#define MAX_NUM		99999999
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
	unsigned int my_num = 719203981;
	int i;
	char buf[BUF_SIZE+1];
	char *new_str = NULL;
	size_t len;

	num_tree = rbtree_new(compare, destroy_str);

	if (!num_tree)
		return -1;

	for (i = 0; i < MAX_TIMES; ++i) {
		my_num += (unsigned int)time(NULL);
		my_num ^= (my_num << 1);
		my_num ^= (my_num >> 3);
		my_num %= MAX_NUM;
		
/*		printf("ins %d\n", my_num); */
		sprintf(buf, "%u", my_num);
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
		my_num += (unsigned int)time(NULL);
		my_num ^= (my_num << 1);
		my_num ^= (my_num >> 3);
		my_num %= MAX_NUM;
/*		printf("del %d\n", my_num); */
		sprintf(buf, "%u", my_num);
		printf("d %s\n", buf);
	
		if (rbtree_remove(num_tree, (void *)buf))
			fprintf(stderr, "error: rbtree_remove: num_tree: data=%s\n", buf);
	}

	
	rbtree_destroy(num_tree);
	num_tree = NULL;
	
	return 0;
}
