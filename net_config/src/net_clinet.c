#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "net_intf.h"


int do_client(int conn)
{
        int data_len;
	int ret;
	struct packet recvbuf;
        struct packet sendbuf;
        memset(&sendbuf, 0, sizeof(sendbuf));
        memset(&recvbuf, 0, sizeof(recvbuf));

        while(fgets(sendbuf.buf, sizeof(sendbuf.buf), stdin) != NULL)
	{
		data_len = strlen(sendbuf.buf);
		sendbuf.len = htonl(data_len);

                writen(conn, &sendbuf, 4+data_len);

                ret = readn(conn, &recvbuf.len, 4);
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

                fputs(recvbuf.buf, stdout);
                memset(&sendbuf, 0, sizeof(sendbuf));
                memset(&recvbuf, 0, sizeof(recvbuf));
        }
	close(conn);
	return 0;
}

int sock_client()
{
        int sock_fd;

        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                ERR_EXIT("service socket");
        }

        struct sockaddr_in sockaddr_client;
        memset(&sockaddr_client, 0, sizeof(sockaddr_client));
        sockaddr_client.sin_family = AF_INET;
        sockaddr_client.sin_port = htons(12345);
        sockaddr_client.sin_addr.s_addr = inet_addr("127.0.0.1") ;

        if(connect(sock_fd, (struct sockaddr*)&sockaddr_client, sizeof(sockaddr_client)) < 0) {
                ERR_EXIT("service connect");
        }

	//do_client(sock_fd);
        int data_len;
        int ret;
        struct packet recvbuf;
        struct packet sendbuf;
        memset(&sendbuf, 0, sizeof(sendbuf));
        memset(&recvbuf, 0, sizeof(recvbuf));

        while(fgets(sendbuf.buf, sizeof(sendbuf.buf), stdin) != NULL)
        {
                data_len = strlen(sendbuf.buf);
                sendbuf.len = htonl(data_len);

                writen(sock_fd, &sendbuf, 4+data_len);

                ret = readn(sock_fd, &recvbuf.len, 4);
                if (ret == -1)
                        ERR_EXIT("read1");
                if (ret < 4)
                {
                        printf("client close\n");
                        break;
                }

                data_len = ntohl(recvbuf.len);
                ret = readn(sock_fd, recvbuf.buf, data_len);
                if (ret == -1)
                        ERR_EXIT("read2");
                if (ret < data_len)
                {
                        printf("client close\n");
                        break;
                }

                fputs(recvbuf.buf, stdout);
                memset(&sendbuf, 0, sizeof(sendbuf));
                memset(&recvbuf, 0, sizeof(recvbuf));
        }
	close(sock_fd);
        return 0;


}

int net_client_main(int argv, char *argc[])
{
	int ret;
	printf("socket client\n");
	ret = sock_client();
	return ret;
}
