#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define log_err(M)	{perror("error: tester: " M); goto error;}


int main(const int argc, const char **argv)
{
	char line[LINE_MAX];
	struct Queue *queue = NULL;
	FILE *conf_file = fopen("test.ini", "r");
	int ret = 0;
	
	if (!(conf_file))
		log_err("fopen conf_file");

	if (!(queue = queue_new(free)))
		goto error;

	while (fgets(line, sizeof(line), conf_file)) {
		size_t len;
		char *new_str;
		
		if (!(len = strlen(line)))
			continue;
		if (!(new_str = malloc(sizeof(*new_str) * (1+len))))
			continue;
		strcpy(new_str, line);

		if (-1 == enqueue(queue, (void *)new_str))
			free(new_str);
	}

	while (!queue_is_empty(queue)) {
		char *data = (char *)dequeue(queue);
		
		if (!data)
			continue;
		
		fprintf(stderr, "dequeue: %s\n", data);
		free(data);
		data = NULL;
	}


out:
	if (conf_file)
		fclose(conf_file);
	if (queue)
		queue_destroy(queue);
	queue = NULL;
	return ret;
error:
	ret = -1;
	goto out;
}
