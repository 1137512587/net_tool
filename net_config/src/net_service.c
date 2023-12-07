#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "net_intf.h"
#include "net_service.h"
#include <signal.h>


void do_service(int conn)
{
        int data_len;
        struct packet recvbuf;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	getpeername(conn, (struct sockaddr*)&client_addr, &client_len);
	printf("ip:%s port:%d connect\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        while(1)
        {
		memset(&recvbuf, 0, sizeof(recvbuf));
                int ret = readn(conn, &recvbuf.len, 4);
		if (ret == -1)
			ERR_EXIT("read");
		if (ret < 4)
		{
			printf("ip:%s port:%d disconnect\n",
					inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			break;
		}

		data_len = ntohl(recvbuf.len);
		ret = readn(conn, recvbuf.buf, data_len);
		if (ret == -1)
                        ERR_EXIT("read");
                if (ret < data_len)
                {
                        printf("ip:%s port:%d disconnect\n",
					inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                        break;
                }
                fputs(recvbuf.buf,stdout);
                writen(conn, &recvbuf, 4+data_len);

        }
}

void head_fun(int sig)
{
	printf("sig test\n");
}

int sock_service()
{
	signal(SIGCHLD,head_fun);
        int sock_fd;
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                ERR_EXIT("clinet socket");
        }

        struct sockaddr_in sockaddr_service;
        memset(&sockaddr_service, 0, sizeof(sockaddr_service));
        sockaddr_service.sin_family = AF_INET;
        sockaddr_service.sin_port = htons(12345);
        sockaddr_service.sin_addr.s_addr = inet_addr("127.0.0.1") ;

        if(bind(sock_fd, (struct sockaddr*)&sockaddr_service, sizeof(sockaddr_service)) < 0)  {
                ERR_EXIT("clinet bind");
        }

        if(listen(sock_fd, SOMAXCONN) < 0) {
                ERR_EXIT("clinet listen");
        }

        int conn_fd;
        struct sockaddr_in sockaddr_client;
        socklen_t client_len = sizeof(sockaddr_client);
	memset(&sockaddr_client, 0, sizeof(sockaddr_client));
	while(1) 
	{
        	if((conn_fd = accept(sock_fd, (struct sockaddr*)&sockaddr_client, &client_len)) < 0) {
               		ERR_EXIT("clinet accept");
        	}
	
		pid_t client_pid;
       		client_pid = fork();
		if (client_pid < 0)
		{
			ERR_EXIT("fork");
		}else if (client_pid == 0) { //子进程
		
			do_service(conn_fd);
			close(conn_fd);
		}

	}

	close(sock_fd);
        
	return 0;

}

int net_service_main(int argv, char *argc[])
{
	int ret;
	printf("socket service\n");
	ret = sock_service();
	return ret;
}
