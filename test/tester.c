#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include <rbmap.h>

static int show_key_val(void *, void *, void *);


int main(const int argc, const char **argv)
{
	RBMap *conf_map;
	char *found;

	conf_map = get_conf_map("test.ini");
	if (!conf_map)
		return -1;

	found = (char *)rbmap_search(conf_map, (const void *)"logs_path");
	if (found)
		printf("main: Yay!... found: %s\n", found);
	
	rbmap_remove(conf_map, (const void *)"logs_path");
	
	if ((found = (char *)rbmap_search(conf_map, (const void *)"logs_path")))
		printf("main: error: Huh!... found again: %s\n", found);
	
	if ((found = (char *)rbmap_search(conf_map, (const void *)"ip_address")))
		printf("main: found IP: %s\n", found);
	if ((found = (char *)rbmap_search(conf_map, (const void *)"server_port")))
		printf("main: found server_port: %s\n", found);

	rbmap_foreach(conf_map, show_key_val, NULL);

	rbmap_destroy(conf_map);
	conf_map = NULL;
	
	return 0;
}


static int show_key_val(void *key, void *val, void *data)
{
	printf("show_key_val: key=%s, val=%s\n", (const char *)key, (const char *)val);

	return 0;
}
