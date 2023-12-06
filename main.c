#include <stdlib.h>
#include <unistd.h>
#include "net_intf.h"
#include "net_property.h"
#include "net_clinet.h"
#include "net_service.h"
#define OPTION_STRING "I:"

struct net_cmd {
	int (*func)(int argc, char *argv[]);
	char * name;
	char * desc;
};

struct net_cmd net_cmd_list[] = {
	{ net_property_main,	"property",	"get net device info." },
	{ net_client_main,	"client",	"socket client." },	 
	{ net_service_main,	"service",	"socket service." },	 
	{ NULL },
};

int net_cmd_run(struct net_cmd * net_cmd_list, char * name,int argc, char * argv[]) 
{
	struct net_cmd * cmdlist = net_cmd_list;
	if (!name) {
		if(!cmdlist->func || !cmdlist->name) {
			printf("have'n func and name\n");
			return -1;
		}
		printf("cmd name is null\n");
	}
	
	for(cmdlist = net_cmd_list; cmdlist->func; cmdlist++){
		if (!strcmp(name, cmdlist->name)) {
			break;
		}
	}

	if (!cmdlist->func) {
		printf("cmdlist have'n func\n");
		return -1;
	}
	
	return cmdlist->func(argc,argv);
}

int net_tool_main(int argc, char *argv[]) {

	int argflag;
	int ret = 0;

	while ((argflag = getopt(argc, argv, OPTION_STRING)) != -1) {
		switch(argflag) {
			case 'I':
				printf("test1\n");
				break;
			default:
				printf("test2\n");
		}
	}
	
	if (argc-optind > 0)
	{
		ret = net_cmd_run(net_cmd_list, argv[optind], argc-optind-1, &(argv[optind+1]));
	}

	return ret;
}

int main(int argc, char *argv[])
{
	int ret;
	
	ret = net_tool_main(argc, argv);

	if (ret == 0)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
	
}
