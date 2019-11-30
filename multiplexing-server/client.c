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
        perror("usage: tcpcli <IPaddress>");
        exit(-1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == -1){
        perror("connect error!");
        return -1;
    }
    send_request(sockfd);
    exit(0);
}

void send_request(int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int msg_length;
    if((msg_length = recv(sockfd, recvline, MAXLINE, 0)) < 0){
        perror("error");
        exit(0);
    }
    recvline[msg_length] = '\0';
    printf("%s\n", recvline);
    while(1)
    {
        printf("Input Words: ");
        scanf("%s", sendline);
        send(sockfd, sendline, strlen (sendline), 0);
        if ((msg_length = recv(sockfd, recvline, MAXLINE, 0)) == 0){
            perror("send_request: server terminated prematurely");
            exit(-1);
        }
        recvline[msg_length] = '\0';
        printf("Your input is: %s\n", recvline);
    }
}
