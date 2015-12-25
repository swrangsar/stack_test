#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "rbmap.h"


#define log_err(M)	{perror("error: config: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: config: " M "\n"); goto error;}

#define KEY_SIZE	512
#define VAL_SIZE	2048
#define LINE_SIZE	(KEY_SIZE + VAL_SIZE)


static int compare(const void *a, const void *b)
{
	if (!a)
		return b?-1:0;
	if (!b)
		return 1;

	return strcmp((const char *)a, (const char *)b);
}

RBMap *get_conf_map(const char *filename)
{
	RBMap *map;
	FILE *conf_file;
	char line[LINE_SIZE];
	char key[KEY_SIZE] = "";
	char val[VAL_SIZE] = "";


	if (!(conf_file = fopen(filename, "r")))
		log_err("get_conf_map: fopen");

	if (!(map = rbmap_new(compare, free, free))) {
		fclose(conf_file);
		log_msg("get_conf_map: rbmap_new null");
	}

	while (fgets(line, sizeof(line), conf_file)) {
		int i;
		int k = 0;
		int key_read = 0;
		size_t key_len;
		size_t val_len;
		char *new_key = NULL;
		char *new_val = NULL;
		char c;

		key[0] = '\0';
		for (i=0; (c = line[i]); ++i) {
			switch (c) {
				case '\n':
					break;

				case '=':
					key[(k < KEY_SIZE - 1)?k:KEY_SIZE-1] = '\0';
					key_read = 1;
					k = 0;
					break;
	
				default:
					if (!key_read && k < KEY_SIZE-1)
						key[k++] = c;
					else if (key_read && k < VAL_SIZE-1)
						val[k++] = c;

					break;
			}
		}

		val[(k < VAL_SIZE-1)?k:VAL_SIZE-1] = '\0';

		fprintf(stderr, "get_conf_map: key=%s, val=%s\n", key, val);
	
		if (!(key_len = strlen(key)))
			continue;
		if (!(val_len = strlen(val)))
			continue;

		new_key = malloc(sizeof(*new_key) * (key_len + 1));
		if (!new_key)
			continue;

		new_val = malloc(sizeof(*new_val) * (val_len + 1));
		if (!new_val) {
			free(new_key);
			new_key = NULL;
			continue;
		}

		strcpy(new_key, key);
		strcpy(new_val, val);

		if ((-1 == rbmap_insert(map, (void *)new_key, (void *)new_val))) {
			free(new_key);
			new_key = NULL;
			free(new_val);
			new_val = NULL;
		}
	}

	
	fclose(conf_file);
	return map;
error:
	return NULL;
}
