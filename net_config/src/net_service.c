#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "net_intf.h"
#include "net_service.h"

int sock_service()
{
	int sock_fd;
	
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ERR_EXIT("service socket");
	}
	
	struct sockaddr_in sockaddr_service;
	memset(&sockaddr_service, 0, sizeof(sockaddr_service));
	sockaddr_service.sin_family = AF_INET;
	sockaddr_service.sin_port = htons(12345);
	sockaddr_service.sin_addr.s_addr = inet_addr("127.0.0.1") ;
	
	if(connect(sock_fd, (struct sockaddr*)&sockaddr_service, sizeof(sockaddr_service)) < 0) {
		ERR_EXIT("service connect");
	}

	char recvbuf[1024] = {0};
	char sendbuf[1024] = {0};
	while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {

		write(sock_fd, sendbuf, sizeof(sendbuf));
		read(sock_fd, recvbuf, sizeof(recvbuf));

		fputs(recvbuf, stdout);
		memset(&sendbuf, 0, sizeof(sendbuf));
		memset(&recvbuf, 0, sizeof(recvbuf));
	}
	return 0;
}

int net_service_main(int argv, char *argc[])
{
	int ret;
	printf("socket service\n");
	ret = sock_service();
	return ret;
}
