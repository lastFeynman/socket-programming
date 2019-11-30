#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<errno.h>
#include<stdarg.h>
#include<syslog.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>
#include<stdlib.h>
#include<sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "server_mt.h"


void *handle_request(void *arg);
void do_get(int sockfd, struct request request_info);
void do_post(int sockfd, struct request request_info);
void do_delete(int sockfd, struct request request_info);
void do_put(int sockfd, struct request request_info);
void bad_request(int sockfd);
void not_found(int sockfd);
void send_response(int sockfd, struct response response_info);
void format_time(char *buf);
void get_modify_time(char *path, char *buf);
void digit_2_str(int a, char* str);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct thread_arg arg;
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
    printf("Web Server is running...\n");
    for ( ; ; ) {
        pthread_t tid;
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, & clilen);
        if (connfd < 0 && connfd == EINTR)
			continue;
		arg.sockfd = connfd;
		inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, arg.ip, 16);
		arg.port = cliaddr.sin_port;

        pthread_create(&tid,NULL,handle_request,(void *)&arg); /* process the request */
    }
}

void *handle_request(void *arg)
{
    ssize_t n;
    char buf[MAXLINE];
    struct client client_info;

    int sockfd = ((struct thread_arg *)arg) -> sockfd;
	memcpy(client_info.ip, ((struct thread_arg *)arg) -> ip, 16);
	client_info.port = ((struct thread_arg *)arg) -> port;

    printf("(%s:%d) connected to the server.\n", client_info.ip, client_info.port);

    struct request request_info;

    while ((n = recv(sockfd, buf, MAXLINE, 0)) > 0){
        int i = 0;
        int j = 0;
        for(i=0; i<METHOD_LENGTH && i<n; i++){
            if(buf[i] == ' ')
                break;
            request_info.method[i] = buf[i];
        }
        if(i < METHOD_LENGTH)
            request_info.method[i] = '\0';
        i += 2;
        for(j=0;j<PATH_MAX_LENGTH && i<n; i++, j++){
            if(buf[i] == ' '){
                break;
            }
            request_info.path[j] = buf[i];
        }
        request_info.path[j] = '\0';
        if(request_info.path[0] == '\0'){
            strcat(request_info.path, "index.html");
        }

        for(;i<n;i++){
            if(buf[i] == '\n')
                break;
        }
        
        if(strcmp(request_info.method, GET) == 0){
            printf("(%s:%d) GET path: %s\n", client_info.ip, client_info.port, request_info.path);
            do_get(sockfd, request_info);
        }else if(strcmp(request_info.method, POST) == 0){
            printf("(%s:%d) POST path: %s\n", client_info.ip, client_info.port, request_info.path);
            do_post(sockfd, request_info);
        }else if(strcmp(request_info.method, DELETE) == 0){
            printf("(%s:%d) DELETE path: %s\n", client_info.ip, client_info.port, request_info.path);
            do_delete(sockfd, request_info);
        }else if(strcmp(request_info.method, PUT) == 0){
            printf("(%s:%d) PUT path: %s\n", client_info.ip, client_info.port, request_info.path);
            do_put(sockfd, request_info);
        }else{
            bad_request(sockfd);
        }
    }
    printf("(%s:%d) left the server.\n", client_info.ip, client_info.port);
    close(sockfd);
}

void do_get(int sockfd, struct request request_info){
    struct response response_info;
    bzero(&response_info, sizeof(response_info));
    response_info.status = OK;
    format_time(response_info.time);

    get_modify_time(request_info.path, response_info.last_modified);

    int req_fd;
    if((req_fd = open(request_info.path, O_RDONLY)) < 0){
        printf("%s Not Found.\n", request_info.path);
        not_found(sockfd);
        return;
    }

    int body_length;
    
    if((body_length = read(req_fd, response_info.content, BODY_MAX_LENGTH)) < 0 || body_length != strlen(response_info.content)){
        printf("Read %s Error. %d, %ld\n", request_info.path, body_length, strlen(response_info.content));
        not_found(sockfd);
        return;
    }
    close(req_fd);
    response_info.content[body_length] = '\0';
    response_info.content_length = body_length;

    send_response(sockfd, response_info);
}

void do_post(int sockfd, struct request request_info){
    do_get(sockfd, request_info);
}

void do_delete(int sockfd, struct request request_info){
    do_get(sockfd, request_info);
}

void do_put(int sockfd, struct request request_info){
    do_get(sockfd, request_info);
}

void bad_request(int sockfd){
    struct response response_info;
    response_info.status = BAD_REQUEST;
    format_time(response_info.time);
    response_info.content_length = 15;
    response_info.content[0] = '\0';
    strcat(response_info.content, "400\nBad Request");

    send_response(sockfd, response_info);
}

void not_found(int sockfd){
    struct response response_info;
    response_info.status = NOT_FOUND;
    format_time(response_info.time);
    response_info.content_length = 13;
    response_info.content[0] = '\0';
    strcat(response_info.content, "404\nNot Found");

    send_response(sockfd, response_info);
}

void send_response(int sockfd, struct response response_info){
    char http_str[BODY_MAX_LENGTH];
    http_str[0] = '\0';
    strcat(http_str, VERSION);
    strcat(http_str, " ");
    switch(response_info.status){
        case OK: 
            strcat(http_str, "200 ");
            strcat(http_str, OK_MSG);
            break;
        case BAD_REQUEST:
            strcat(http_str, "400 ");
            strcat(http_str, BAD_REQUEST_MSG);
            break;
        default:
            strcat(http_str, "404 ");
            strcat(http_str, NOT_FOUND_MSG);
            break;
    }
    strcat(http_str, "\r\nDate: ");
    strcat(http_str, response_info.time);
    strcat(http_str, "\r\n");
    strcat(http_str, SERVER);
    strcat(http_str, "\r\n");
    if(response_info.status == OK){
        strcat(http_str, "Last-Modified: ");
        strcat(http_str, response_info.last_modified);
        strcat(http_str, "\r\n");
    }
    strcat(http_str, "Content-Length: ");
    char digit_str[5];
    digit_2_str(response_info.content_length, digit_str);
    strcat(http_str, digit_str);
    strcat(http_str, "\r\n");
    strcat(http_str, CONTENT_TYPE);
    strcat(http_str, "\r\n");
    strcat(http_str, CONNECTION);
    strcat(http_str, "\r\n\r\n");
    strcat(http_str, response_info.content);
    send(sockfd, http_str, strlen(http_str), 0);
}

void format_time(char *buf){
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, TIME_LENGTH, DATE_FORMAT, &tm);
}

void get_modify_time(char *path, char *buf) {
    struct stat attr;
    stat(path, &attr);
    strftime(buf, TIME_LENGTH, DATE_FORMAT, gmtime(&attr.st_mtime));
}

void digit_2_str(int a, char* str){
    int digit;
    int i = 0;
    while(a > 0){
        digit = a % 10;
        a /= 10;
        str[i] = digit+48;
        i++;
    }
    int length = i;
    str[length] = '\0';
    char t;
    for(i=0;i<length/2;i++){
        t = str[i];
        str[i] = str[length-1-i];
        str[length-1-i] = t;
    }
}