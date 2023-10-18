#ifndef __NET_INTF__
#define __NET_INTF__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define ERR_EXIT(m)\
	do\
	{	\
		perror(m);\
		exit(EXIT_FAILURE);\
	} while(0)

#endif
