#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include "rbmap.h"



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

	rbmap_destroy(conf_map);
	conf_map = NULL;
	
	return 0;
}
