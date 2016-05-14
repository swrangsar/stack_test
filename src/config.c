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


static int compare(const void *, const void *);
static void process_line(const char *, RBMap *);


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
	char line[LINE_SIZE];
	RBMap *map = NULL;
	FILE *conf_file = NULL;

	if (!(conf_file = fopen(filename, "r")))
		log_err("get_conf_map: fopen");

	if (!(map = rbmap_new(compare, free, free)))
		log_msg("get_conf_map: rbmap_new null");

	while (fgets(line, sizeof(line), conf_file))
		process_line(line, map);
	
error:
	if (conf_file)
		fclose(conf_file);
	return map;
}

static void process_line(const char *line, RBMap *map)
{
	char key[KEY_SIZE];
	char val[VAL_SIZE];
	int got_eol;
	int k;
	int i;
	int key_read = 0;
	size_t key_len;
	size_t val_len;
	const char *c;
	char *new_key = NULL;
	char *new_val = NULL;

	if (!line)
		return;
	if (!map)
		return;

	got_eol = 0;
	k = 0;
	i = 0;
	for (c = line; !got_eol ; ++c) {
		switch (*c) {
			case '\0':
			case '#':
			case '\n':
				got_eol = 1;
				break;

			case '=':
				key_read = 1;
				break;

			default:
				if (!key_read && k < KEY_SIZE-1)
					key[k++] = *c;
				else if (key_read && i < VAL_SIZE-1)
					val[i++] = *c;
				break;
		}
	}

	key[(k < KEY_SIZE - 1)?k:KEY_SIZE-1] = '\0';
	val[(i < VAL_SIZE-1)?i:VAL_SIZE-1] = '\0';


	if (!(key_len = strlen(key)))
		return;
	if (!(val_len = strlen(val)))
		return;
	if (!(new_key = malloc(sizeof(*new_key) * (key_len + 1))))
		goto error;
	if (!(new_val = malloc(sizeof(*new_val) * (val_len + 1))))
		goto error;

	strcpy(new_key, key);
	strcpy(new_val, val);

	if (-1 == rbmap_insert(map, (void *)new_key, (void *)new_val))
		goto error;

	fprintf(stderr, "get_conf_map: key=%s, val=%s\n", key, val);
	return;
error:
	free(new_key);
	free(new_val);
}
