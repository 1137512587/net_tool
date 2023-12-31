#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "net_intf.h"
#include "net_service.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
/*
@addr:服务端地址
@port:服务端端口
@return:成功返回sock描述
*/
int sock_service_init(const char *addr,unsigned int port)
{
        int sock_fd;
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                ERR_EXIT("clinet socket");
        }

        struct sockaddr_in sockaddr_service;
        memset(&sockaddr_service, 0, sizeof(sockaddr_service));
        sockaddr_service.sin_family = AF_INET;
        sockaddr_service.sin_port = htons(port);
        sockaddr_service.sin_addr.s_addr = inet_addr(addr) ;

        if(bind(sock_fd, (struct sockaddr*)&sockaddr_service, sizeof(sockaddr_service)) < 0)  {
                ERR_EXIT("clinet bind");
        }


        if(listen(sock_fd, SOMAXCONN) < 0) {
                ERR_EXIT("clinet listen");
        }
	
	return sock_fd;
}

/*
 *服务端处理
 *@conn:已连接套接字
 *
 */
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
/*
 *多进程处理并发
 *@listenfd：侦听套接字
 *
 */
int sock_fork(int listenfd)
{
        int conn_fd;
        pid_t client_pid;
        struct sockaddr_in sockaddr_client;
        socklen_t client_len = sizeof(sockaddr_client);
        memset(&sockaddr_client, 0, sizeof(sockaddr_client));

        while(1)
        {
                if((conn_fd = accept(listenfd, (struct sockaddr*)&sockaddr_client, &client_len)) < 0) {
                        ERR_EXIT("clinet accept");
                }

                client_pid = fork();
                if (client_pid < 0)
                {
                        ERR_EXIT("fork");
                }else if (client_pid == 0) { //子进程

                        do_service(conn_fd);
                        close(conn_fd);
                        exit(EXIT_SUCCESS);
                }
        }
	return 0;
}

int sock_select(int listenfd)
{
	
	int ret;
	int nready;
	int conn_fd;
        struct sockaddr_in sockaddr_client;
        socklen_t client_len = sizeof(sockaddr_client);
        memset(&sockaddr_client, 0, sizeof(sockaddr_client));
	int data_len;
	struct packet recvbuf;

	int maxfd = listenfd;
	int maxi = 0;
	fd_set allset;
	fd_set rset;
	FD_ZERO(&allset);
	FD_ZERO(&rset);
	FD_SET(listenfd, &allset);
        
	int i;
        int client[FD_SETSIZE];
        for (i=0; i<FD_SETSIZE; i++)
                 client[i] = -1;	
	while(1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (nready == -1)
		{
			if (errno == EINTR)
				continue;
			ERR_EXIT("select fail");
		} else if (nready == 0)
		{
			continue;
		}
		
		if (FD_ISSET(listenfd, &rset))
		{
	                if((conn_fd = accept(listenfd, (struct sockaddr*)&sockaddr_client, &client_len)) < 0) {
        	                ERR_EXIT("clinet accept");
               		 }
			
			for (i=0; i<FD_SETSIZE; i++){
				if (client[i] < 0) {
					client[i] = conn_fd;
					if (i > maxi)
						maxi = i;
					break;
				}
			}
			
			if (i == FD_SETSIZE)
			{
				fprintf(stderr,"too may client.\n");
				exit(EXIT_FAILURE);
			}

			printf("ip:%s port:%d connect\n",
                                        inet_ntoa(sockaddr_client.sin_addr), ntohs(sockaddr_client.sin_port));
                        
			FD_SET(conn_fd,&allset);
                        if (conn_fd > maxfd)
                                maxfd = conn_fd;

			if (--nready <= 0)
				continue;
		}
		
		for(i=0; i<=maxi; i++)
		{
			conn_fd = client[i];
			if (conn_fd == -1)
				continue;

			if (FD_ISSET(conn_fd, &rset))
			{
				memset(&sockaddr_client, 0, sizeof(sockaddr_client));
				getpeername(conn_fd, (struct sockaddr*)&sockaddr_client, &client_len);		
				memset(&recvbuf, 0, sizeof(recvbuf));
				ret = readn(conn_fd, &recvbuf.len, 4);
                		if (ret == -1)
                        		ERR_EXIT("read");
                		if (ret < 4)
                		{
                        		printf("ip:%s port:%d disconnect\n",
                                       		inet_ntoa(sockaddr_client.sin_addr), ntohs(sockaddr_client.sin_port));
                        		FD_CLR(conn_fd,&allset);
					client[i] = -1;
					continue;
                		}

                		data_len = ntohl(recvbuf.len);
                		ret = readn(conn_fd, recvbuf.buf, data_len);
                		if (ret == -1)
                        		ERR_EXIT("read");
                		if (ret < data_len)
                		{
                        		printf("ip:%s port:%d disconnect\n",
                                        	inet_ntoa(sockaddr_client.sin_addr), ntohs(sockaddr_client.sin_port));
                                        FD_CLR(conn_fd,&allset);
					client[i] = -1;
                                        continue;
                        		
                		}
                		fputs(recvbuf.buf,stdout);
                		writen(conn_fd, &recvbuf, 4+data_len);
				if (--nready <= 0)
                                	break;
				
			}
		}
		

	}
	return 0;
}

void header_fun(int sig)
{
	printf("sig test\n");
	while (waitpid(-1, NULL,WNOHANG) > 0);
}

int sock_service()
{
	signal(SIGCHLD,header_fun);


	int listenfd;
	listenfd = sock_service_init("127.0.0.1",12345);
	//(void)sock_fork(listenfd);
	(void)sock_select(listenfd);
	close(listenfd);
   
	return 0;

}

int net_service_main(int argv, char *argc[])
{
	int ret;
	printf("socket service\n");
	ret = sock_service();
	return ret;
}
