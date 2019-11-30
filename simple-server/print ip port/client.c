#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<errno.h>
#include<stdarg.h>
#include<syslog.h>
#include<stdio.h>

#define SERV_PORT 9877
#define	SA	struct sockaddr
#define	LISTENQ		1024
#define	MAXLINE		4096
//#define	LOG_ERR		3

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    if (argc != 2){
        printf("usage: tcpcli <IPaddress>");
        exit(-1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == -1)
        printf("%d", errno);
    exit(0);
}
