#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"


static int show_key_val(void *, void *, void *);


int main(const int argc, const char **argv)
{
	struct rbtree conf_map = {NULL,};
	char *found;

	if (get_conf_map("test.ini", &conf_map))
		return -1;

	found = (char *)rbtree_search(&conf_map, (const void *)"logs_path");
	if (found)
		printf("main: Yay!... found: %s\n", found);
	
	rbtree_remove(&conf_map, (const void *)"logs_path");
	
	if ((found = (char *)rbtree_search(&conf_map, (const void *)"logs_path")))
		printf("main: error: Huh!... found again: %s\n", found);
	
	if ((found = (char *)rbtree_search(&conf_map, (const void *)"ip_address")))
		printf("main: found IP: %s\n", found);
	if ((found = (char *)rbtree_search(&conf_map, (const void *)"server_port")))
		printf("main: found server_port: %s\n", found);

	rbtree_foreach(&conf_map, show_key_val, NULL);

	rbtree_destroy(&conf_map);
	
	return 0;
}


static int show_key_val(void *key, void *val, void *data)
{
	printf("show_key_val: key=%s, val=%s\n", (const char *)key, (const char *)val);

	return 0;
}
