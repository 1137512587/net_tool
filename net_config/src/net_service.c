#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "net_intf.h"
#include "net_service.h"


void do_service(int conn)
{
        int data_len;
        struct packet recvbuf;
        while(1)
        {
		memset(&recvbuf, 0, sizeof(recvbuf));
                int ret = readn(conn, recvbuf.len, 4);
		if (ret == -1)
			ERR_EXIT("read");
		if (ret < 4)
		{
			printf("client close\n");
			break;
		}

		data_len = ntohl(recvbuf.len);
		ret = readn(conn, recvbuf.buf, data_len);
		if (ret == -1)
                        ERR_EXIT("read");
                if (ret < data_len)
                {
                        printf("client close\n");
                        break;
                }
                fputs(recvbuf.buf,stdout);
                writen(conn, &recvbuf, 4+data_len);

        }
}


int sock_service()
{
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
        if((conn_fd = accept(sock_fd, (struct sockaddr*)&sockaddr_client, &client_len)) < 0) {
                ERR_EXIT("clinet accept");
        }
        printf("ip:%d port:%s connect\n", ntohs(sockaddr_client.sin_port), inet_ntoa(sockaddr_client.sin_addr));
	
	do_service(conn_fd);
        
	return 0;

}

int net_service_main(int argv, char *argc[])
{
	int ret;
	printf("socket service\n");
	ret = sock_service();
	return ret;
}
