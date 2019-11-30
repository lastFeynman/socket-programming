#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#define SERV_PORT 9877
#define MAXLINE 100
#define LISTENQ 100

void *handle_request(void *arg);

struct ARG {
	int connfd;
	char ip[16];
	int port
};

int main(int argc, char *argv[]) {
	int listenfd, connfd;
	size_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	struct ARG arg;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	printf("server is running...\n");
	listen(listenfd, LISTENQ);

	while (1) {
		pthread_t tid;
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
		if (connfd < 0 && connfd == EINTR)
			continue;
		arg.connfd = connfd;
		inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, arg.ip, 16);
		arg.port = cliaddr.sin_port;
		pthread_create(&tid,NULL,handle_request,(void *)&arg);
	}
	return 0;
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

void *handle_request(void *arg)
{
	ssize_t n;
    char buf[MAXLINE];
    char welcome_str[] = "Welcome\n";
	char all_input[MAXLINE*4];
	all_input[0] = '\0';
	
	pthread_detach(pthread_self());

	int sockfd = ((struct ARG *)arg) -> connfd;
	char ip[16];
	memcpy(ip, ((struct ARG *)arg) -> ip, 16);
	int port = ((struct ARG *)arg) -> port;
	printf("successfully connect %s:%d\n", ip, port);
	send(sockfd, welcome_str, strlen(welcome_str), 0);

	while ((n = recv(sockfd, buf, MAXLINE, 0)) > 0){
		strcat(all_input, buf);
		strcat(all_input, "\t");
        buf[n] = '\0';
        printf("Receive from (%s:%d): %s\n", ip, port, buf);
        reverse(buf);
        send(sockfd, buf, n, 0);
    }

	printf("%s:%d is offline now. All its inputs are: %s\n", ip, port, all_input);
	close(sockfd);
	return NULL;
}
