#include "rtmon.h"
#include <stdio.h>
#include <stdlib.h>


#define log_err(M)      {perror("error: rtmon: " M); goto error;}
#define log_msg(M)      {fprintf(stderr, "error: rtmon: " M "\n"); goto error;}


static char **tokenize(char *, int, size_t *);


static char **tokenize(char *str, int sep, size_t *n_toks)
{
	char **tokens;
	size_t capacity = 1;
	size_t used = 0;
	int i = 0;
	int flag = 0;
	char c;
	char *start = str;
	char **tmp;
	
	if (!(tokens = calloc(capacity, sizeof(*tokens))))
		log_err("tokenize: tokens calloc");

	do {
		if (!flag) {
			start = &str[i];
			flag = 1;
		}
		c = str[i++];
		
		if (c && c != (char)sep && '\n' != c)
			continue;

		str[i-1] = '\0';

		if (used == capacity) {
			capacity *= 2;
			if (!(tmp = realloc(tokens, capacity * sizeof(*tokens))))
				log_err("tokenize: tokens realloc");
			tokens = tmp;
		}

		tokens[used++] = start;
		flag = 0;
	} while (c && '\n' != c);

error:
	*n_toks = used;
	return tokens;
}
