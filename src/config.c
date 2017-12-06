#include "config.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rbmap.h"
#include "logmsg.h"


#define KEY_SIZE	512
#define VAL_SIZE	512


static int compare(const void *, const void *);
static void process_line(const char *, struct RBMap *);


static int compare(const void *a, const void *b)
{
	if (!a)
		return b?-1:0;
	if (!b)
		return 1;

	return strcmp((const char *)a, (const char *)b);
}

int get_conf_map(const char *filename, struct RBMap *map)
{
	int ret_val = -1;
	char line[LINE_MAX];
	FILE *conf_file = NULL;

	if ((conf_file = fopen(filename, "r"))) {
		if ((!rbmap_init(map, compare, free, free))) {
			while (fgets(line, sizeof(line), conf_file))
				process_line(line, map);
			ret_val = 0;
		} else {
			log_err("get_conf_map: rbmap_new null\n");
		}

		fclose(conf_file);
	} else {
		log_err("get_conf_map: fopen");
	}

	return ret_val;
}

static void process_line(const char *line, struct RBMap *map)
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
