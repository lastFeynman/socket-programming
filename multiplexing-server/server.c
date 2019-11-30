#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/select.h>
#include<stddef.h>

#define MAXLINE 1024
#define SERV_PORT 9877
#define LISTENQ 1024

struct CLIENT {
	int sockfd;
	char ip[16];
	int port;
	char content[MAXLINE * 4]
};

int main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready;
	struct CLIENT client[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	printf("server is running...\n");
	listen(listenfd, LISTENQ);

	maxfd = listenfd;
	maxi = -1;
	for (i = 0; i < FD_SETSIZE; i++)
		client[i].sockfd = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for ( ; ; ) {
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct servaddr *) &cliaddr, &clilen);

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i].sockfd < 0) {
					client[i].sockfd = connfd;
					inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, client[i].ip, 16);
					client[i].port = cliaddr.sin_port;
					client[i].content[0] = '\0';
					send(connfd, "Welcome", 7, 0);
					printf("successfully connect to %s:%d\n", client[i].ip, client[i].port);
					break;
				}
			if (i == FD_SETSIZE){
				printf("too many clients\n");
				exit(0);
			}

			FD_SET(connfd, &allset);
			if (connfd > maxfd)
				maxfd = connfd;
			if (i > maxi)
				maxi = i;

			if (--nready <= 0)
				continue;
		}



		for (i = 0; i <= maxi; i++) {
			if ( (sockfd = client[i].sockfd) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = recv(sockfd, buf, MAXLINE, 0)) == 0) {
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i].sockfd = -1;
					printf("%s:%d is offline now. All its inputs are: %s\n", client[i].ip, client[i].port, client[i].content);
				} else{
					buf[n] = '\0';
					printf("Receive from (%s:%d):%s\n", client[i].ip, client[i].port, buf);
					strcat(client[i].content, buf);
					strcat(client[i].content, "\t");
					send(sockfd, buf, n, 0);
				}

				if (--nready <= 0)
					break;
			}
		}
	}
}
