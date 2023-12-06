#include "net_intf.h"

//封装read和write函数解决流式套接字粘包问题

ssize_t readn(int fd, void *buf, size_t count)
{
	ssize_t nleft = count;
	ssize_t nread;
	char *bufp =(char*)buf;

	while (nleft > 0)
	{
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR) //信号中断
				continue;
			return -1;
		}
		else if (nread == 0) //对方关闭套接字
			return count - nleft; 

		bufp += nread;
		nleft -= nread;

	}
	return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
	ssize_t nleft = count;
        ssize_t nwrite;
        char *bufp =(char*)buf;

        while (nleft > 0)
        {
                if ((nwrite = write(fd, bufp, nleft)) < 0)
                {
                        if (errno == EINTR) //信号中断
                                continue;
                        return -1;
                }
                else if (nwrite == 0)
                        continue;

                bufp += nwrite;
                nleft -= nwrite;

        }
        return count;
}
