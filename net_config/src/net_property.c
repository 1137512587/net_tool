#include "net_intf.h"
#include "net_property.h"

#define DEV_NAME_PATH "/proc/net/dev"

int net_dev_name()
{
	FILE *fp;
	char buf[1024] = {0};
	
	fp = fopen(DEV_NAME_PATH, "r");
	if (fp == NULL) {
		ERR_EXIT("fopen fail");
	}

	while (fgets(buf,sizeof(buf), fp) != NULL) {
		printf("%s",buf);
	}
	
	fclose(fp);
	return 0;
}


int net_property_main(int argv, char *argc[])
{
	int ret;
	ret = net_dev_name();
	return ret;
}
