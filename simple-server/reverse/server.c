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
    if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) == -1){
        perror("bind error!");
        return -1;
    }
    if(listen(listenfd, LISTENQ) == -1){
        perror("listen error!");
        return -1;
    }
    printf("Server is running...\n");
    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, & clilen);
        handle_request(connfd, cliaddr); /* process the request */
        close(connfd);
    }
}

void reverse(char buf[]){
    int n = strlen(buf);
    int i;
    char c;
    for(i=0;i<n/2;i++){
        c = buf[i];
        buf[i] = buf[n-i-1];
        buf[n-i-1] = c;
    }
}

void print_client_address(struct sockaddr_in cliaddr)
{
    char buf[] = "IP:";
    char ip[16];
    inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, 16);
    strcat(buf, ip);
    strcat(buf, "\tPORT:");
    printf("%s%d\n", buf, cliaddr.sin_port);
}

void handle_request(int sockfd, struct sockaddr_in cliaddr)
{
    ssize_t n;
    char buf[MAXLINE];
    char welcome_str[] = "Welcome\n";

    print_client_address(cliaddr);
    send(sockfd, welcome_str, strlen(welcome_str), 0);
    while ((n = recv(sockfd, buf, MAXLINE, 0)) > 0){
        buf[n] = '\0';
        printf("Receive: %s", buf);
        reverse(buf);
        send(sockfd, buf, n, 0);
    }
}
