#include "net_intf.h"
#include "net_property.h"

#define DEV_NAME_PATH "/proc/net/dev"

struct property_node {
	char index;
	char dev_name[10];
};

int net_dev_name()
{
	FILE *fp;
	char buf[1024] = {0};
	char name[10] = {0};
	int i,j;
	int dev_num = 0;
	struct property_node net_property[2];
	
	fp = fopen(DEV_NAME_PATH, "r");
	if (fp == NULL) {
		ERR_EXIT("fopen fail");
	}

	(void *)fgets(buf,sizeof(buf), fp);
	(void *)fgets(buf,sizeof(buf), fp);

	while (fgets(buf,sizeof(buf), fp) != NULL) {
		j=0;
		for(i = 0;i < sizeof(buf);i++)
		{
			if(buf[i] == ' ')
				continue;
			else if(buf[i] == ':')
				break;
			else
				name[j] = buf[i];
			j++;
		}
		net_property[dev_num].index = dev_num;
		//net_property[dev_num].dev_name = name;
		//存在数组内容被误修改的情况
		strcpy(net_property[dev_num].dev_name,name);
		dev_num++;
	}
	
	for(i = 0;i < dev_num;i++) {
		printf("index:%d name:%s\n",net_property[i].index,net_property[i].dev_name);
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
