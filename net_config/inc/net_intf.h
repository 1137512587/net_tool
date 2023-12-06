#ifndef __NET_INTF__
#define __NET_INTF__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define ERR_EXIT(m)\
	do\
	{	\
		perror(m);\
		exit(EXIT_FAILURE);\
	} while(0)

//包头包体结构
struct packet
{
	int len;
	char buf[1024];
};

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);

#endif
