#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<errno.h>
#include<stdarg.h>
#include<syslog.h>
#include<stdio.h>
#include<string.h>

#define SERV_PORT 9877
#define	SA	struct sockaddr
#define	LISTENQ		1024
#define	MAXLINE		4096
//#define	LOG_ERR		3

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    listenfd = socket (AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (SERV_PORT);
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    if(listen(listenfd, LISTENQ) == -1){
        printf("listen error");
        exit(-1);
    }
    while(1) {
        clilen = sizeof(cliaddr);
        if(connfd = accept(listenfd, (SA *) &cliaddr, & clilen) == -1)
            fputs("accept error", stdout);
        handle_request(connfd, cliaddr); /* process the request */
        close(connfd);
    }
}

void handle_request(int sockfd, struct sockaddr_in cliaddr)
{
    char buf[] = "IP:";
    char ip[16];
    inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, 16);
    strcat(buf, ip);
    strcat(buf, "\tPORT:");
    printf("%s%d\n", buf, cliaddr.sin_port);
}
