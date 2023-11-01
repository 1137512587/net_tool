#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "net_intf.h"


int sock_clinet()
{
	int sock_fd;
	
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ERR_EXIT("clinet socket");
	}
	
	struct sockaddr_in sockaddr_clinet;
	memset(&sockaddr_clinet, 0, sizeof(sockaddr_clinet));
	sockaddr_clinet.sin_family = AF_INET;
	sockaddr_clinet.sin_port = htons(12345);
	sockaddr_clinet.sin_addr.s_addr = inet_addr("127.0.0.1") ;

	if(bind(sock_fd, (struct sockaddr*)&sockaddr_clinet, sizeof(sockaddr_clinet)) < 0)  {
		ERR_EXIT("clinet bind");
	}

	if(listen(sock_fd, SOMAXCONN) < 0) {
		ERR_EXIT("clinet listen");
	}
	
	int conn_fd;
	struct sockaddr_in sockaddr_service;
	socklen_t service_len = sizeof(sockaddr_service);
	if((conn_fd = accept(sock_fd, (struct sockaddr*)&sockaddr_service, &service_len)) < 0) {
		ERR_EXIT("clinet accept");
	}

	char recvbuf[1024];
	while(1) {
		memset(recvbuf, 0, sizeof(recvbuf));
		int ret = read(conn_fd, recvbuf, sizeof(recvbuf));
		fputs(recvbuf,stdout);
		write(conn_fd,recvbuf,ret);
	}
	return 0;
}

int net_clinet_main(int argv, char *argc[])
{
	int ret;
	printf("socket clinet\n");
	ret = sock_clinet();
	return ret;
}
