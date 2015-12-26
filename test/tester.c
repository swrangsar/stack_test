#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rbset.h>

#define log_msg(M)	{fprintf(stderr, "error: rbset_test: " M "\n"); goto error;}

static int show_key(void *, void *); 
static int compare(const void *, const void *);


int main(const int argc, const char **argv)
{
	RBSet *conf_set;
	int ret = 0;
	char *new_str = NULL;

	if (!(conf_set = rbset_new(compare, free)))
		return -1;
	
	if (!(new_str = malloc(sizeof(*new_str) * 20)))
		goto error;
	sprintf(new_str, "%.*s", 19, "logs_path");
	if (-1 == rbset_insert(conf_set, (void *)new_str))
		log_msg("insert: logs_path!");

	if (!(new_str = malloc(sizeof(*new_str) * 20)))
		goto error;
	sprintf(new_str, "%.*s", 19, "ip_address");
	if (-1 == rbset_insert(conf_set, (void *)new_str))
		log_msg("insert: ip_address!");

	if (!(new_str = malloc(sizeof(*new_str) * 20)))
		goto error;
	sprintf(new_str, "%.*s", 19, "server_port");
	if (-1 == rbset_insert(conf_set, (void *)new_str))
		log_msg("insert: server_port!");

	if (!(new_str = malloc(sizeof(*new_str) * 20)))
		goto error;
	sprintf(new_str, "%.*s", 19, "msg_line_width");
	if (-1 == rbset_insert(conf_set, (void *)new_str))
		log_msg("insert: msg_line_width!");

	if (!(new_str = malloc(sizeof(*new_str) * 20)))
		goto error;
	sprintf(new_str, "%.*s", 19, "bin_path");
	if (-1 == rbset_insert(conf_set, (void *)new_str))
		log_msg("insert: bin_path!");

	if (!rbset_search(conf_set, (const void *)"logs_path"))
		printf("main: Yay!... found logs_path\n");
	
	rbset_remove(conf_set, (const void *)"logs_path");
	
	if (!rbset_search(conf_set, (const void *)"logs_path"))
		printf("main: error: Huh!... found again: logs_path\n");
	
	if (!rbset_search(conf_set, (const void *)"ip_address"))
		printf("main: found ip_address\n");
	if (!rbset_search(conf_set, (const void *)"server_port"))
		printf("main: found server_port\n");

	rbset_foreach(conf_set, show_key, NULL);

out:
	if (conf_set)
		rbset_destroy(conf_set);
	return ret;
error:
	if (new_str)
		free(new_str);
	ret = 1;
	goto out;
}


static int show_key(void *key, void *data)
{
	printf("show_key: key=%s\n", (const char *)key);

	return 0;
}

static int compare(const void *a, const void *b)
{
	if (!a)
		return b?-1:0;
	if (!b)
		return 1;
	return strcmp((const char *)a, (const char *)b);
}
