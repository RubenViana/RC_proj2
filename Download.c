#include "Download.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

//Only for testing while parsing not implemented !

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"


int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: Download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    info h_info;

    if (parseInput(argv[1], &h_info) == -1) {
        fprintf(stderr, "Usage: Download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    if (getIp(h_info.host_name, &h_info) == -1) {
        fprintf(stderr, "Error on getIp\n");
        exit(-1);
    }

    //Create connection
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(h_info.ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }
    //Connection established

    char cmd[512];

    //Login
    sprintf(cmd, "user %s\n", h_info.user);
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD error user");
        exit(-1);
    }
    if (readResp(sockfd) == -1) {
        perror("Resp error user");
        exit(-1);
    }

    sprintf(cmd, "pass %s\n", h_info.pass);
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD error pass");
        exit(-1);
    }
    if (readResp(sockfd) == -1) {
        perror("Resp error user");
        exit(-1);
    }

    //Set to passive
    sprintf(cmd, "pasv\n");
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD pasv error");
        exit(-1);
    }
    if (readRespPasv(sockfd) == -1) {
        perror("Resp error user");
        exit(-1);
    }




    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    return 0;
}

int getIp(char* hostName, info* h_info) {
    struct hostent *h;

    if ((h = gethostbyname(hostName)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    strcpy(h_info->host_name, h->h_name);
    strcpy(h_info->ip, h->h_addr);

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

    return 0;
}

int sendCMD(int sockfd, char* cmd) {
    int bytes = write(sockfd, cmd, strlen(cmd));
    if (bytes > 0)
        printf("Bytes escritos %d\n", bytes);
    else {
        perror("sendCMD");
        return(-1);
    }
    return 0;
}

int readResp(int sockfd) {



    return 0;
}

int readRespPasv(int sockfd) {



    return 0;
}

int parseInput(char* input, info* h_info) {


    return 0;
}