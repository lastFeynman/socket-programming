#define SERV_PORT 8080
#define	SA	struct sockaddr
#define	LISTENQ		1024
#define	MAXLINE		4096
#define IP_LENGTH   16
#define METHOD_LENGTH 10
#define PATH_MAX_LENGTH 200
#define BODY_MAX_LENGTH 500000
#define TIME_LENGTH 50

#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"
#define PUT "PUT"

#define VERSION "HTTP/1.1"


#define OK 200
#define OK_MSG "OK"

#define BAD_REQUEST 400
#define BAD_REQUEST_MSG "Bad Request"

#define NOT_FOUND 404
#define NOT_FOUND_MSG "Not Found"

#define DATE_FORMAT "%a, %d %b %Y %H:%M:%S %Z"
#define SERVER "Server: Web Server/1.0"
#define CONTENT_TYPE "Content-Type: text/html"
#define CONNECTION "Connection: Closed"

struct client {
    int sockfd;
    char ip[IP_LENGTH];
    int port;
};

struct request {
    char method[METHOD_LENGTH];
    char path[PATH_MAX_LENGTH];
    char body[BODY_MAX_LENGTH];
};

struct response {
    int status;
    char time[TIME_LENGTH];
    char last_modified[TIME_LENGTH];
    int content_length;
    char content[BODY_MAX_LENGTH];
};

struct thread_arg {
    int sockfd;
    char ip[16];
	int port;
};