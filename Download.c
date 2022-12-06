#include "Download.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <regex.h>
#include <fcntl.h>  


int main(int argc, char **argv) {

    FILE* sockFile;
    int sockfd;
    int sockfdrecv;
    info h_info;

    if (argc != 2) {
        fprintf(stderr, "Usage: Download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    if (parseInput(argv[1], &h_info) == -1) {
        fprintf(stderr, "Usage: Download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    if (getIp(&h_info) == -1) {
        fprintf(stderr, "Error on getIp\n");
        exit(-1);
    }

    printf("\n[!] DOWNLOAD PARAMETERS :\n");
    printf("    - Host: %s\n", h_info.host);
    printf("    - User: %s\n", h_info.user);
    printf("    - Password: %s\n", h_info.pass);
    printf("    - Path: %s\n", h_info.path);
    printf("    - Filename: %s\n\n", h_info.file_name);

    //Open connection 1
    if (openConnection(&sockfd, h_info.ip, SERVER_PORT) == -1) {
        fprintf(stderr, "Error on openConnection\n");
        exit(-1);
    }
    printf("[+] CONNECTION ESTABLISHED  [%s:%d]\n", h_info.ip, SERVER_PORT);

    sockFile = fdopen(sockfd,"r");
    recvMSG(sockFile);
    
    char cmd[516];

    //Login set user
    sprintf(cmd, "user %s\n", h_info.user);
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD error user");
        exit(-1);
    }
    if (recvMSG(sockFile) == -1) {
        perror("MSG error user");
        exit(-1);
    }

    //Login set pass
    sprintf(cmd, "pass %s\n", h_info.pass);
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD error pass");
        exit(-1);
    }
    if (recvMSG(sockFile) == -1) {
        perror("MSG error pass");
        exit(-1);
    }

    //Set to passive
    sprintf(cmd, "pasv\n");
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD error pasv");
        exit(-1);
    }

    char iprecv[100];
    char portrecv[100];

    if (recvMSGpasv(sockFile, iprecv, portrecv) == -1) {
        perror("MSG error pasv");
        exit(-1);
    }

    //open connection 2
    if (openConnection(&sockfdrecv, iprecv, atoi(portrecv)) == -1) {
        fprintf(stderr, "Error on openConnection\n");
        exit(-1);
    }
    printf("[+] CONNECTION ESTABLISHED  [%s:%s]\n", iprecv, portrecv);

    //Set to retr
    sprintf(cmd, "retr %s\n", h_info.path);
    if (sendCMD(sockfd, cmd) == -1) {
        perror("CMD error pasv");
        exit(-1);
    }
    if (recvMSG(sockFile) == -1) {
        perror("MSG error pass");
        exit(-1);
    }

    if (saveToFile(sockfdrecv, h_info.file_name) == -1) {
        perror("Save to file error");
        exit(-1);
    }
    printf("\n[+] FILE SAVED IN CWD  [%s]\n\n", h_info.file_name);

    if (closeConnection(sockfdrecv) == -1) exit(-1);
    printf("[+] CONNECTION CLOSE  [%s:%s]\n", iprecv, portrecv);

    if (closeConnection(sockfd) == -1) exit(-1);
    printf("[+] CONNECTION CLOSE  [%s:%d]\n", h_info.ip, SERVER_PORT);
    
    return 0;
}

int openConnection(int* fd, char* ip, int port) {
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    /*connect to the server*/
    if (connect(*fd,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }

    return 0;
}

int closeConnection(int fd) {
    if (close(fd)<0) {
        perror("close()");
        return -1;
    }
    return 0;
}

int getIp(info* h_info) {
    struct hostent *h;

    if ((h = gethostbyname(h_info->host)) == NULL) {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(h_info->host_name, h->h_name);
    strcpy(h_info->ip, inet_ntoa(*((struct in_addr *) h->h_addr)));

    return 0;
}

int sendCMD(int sockfd, char* cmd) {
    int bytes = send(sockfd, cmd, strlen(cmd), 0);
    if (bytes < 0) {
        perror("sendCMD");
        return(-1);
    }
    return 0;
}

int recvMSG(FILE* fd) {
    char *buf;
    size_t rbytes = 0;
    while (1){
        getline(&buf, &rbytes, fd);
        //printf("< %s", buf);
        if (buf[3] == ' '){
            long code = strtol(buf, &buf, 10);
            //printf("code: %d\n", code);
            if (code == 550 || code == 530)
            {
                printf("Command error\n");
                return -1;
            }
            break;
        }
    }
    return 0;
}

int recvMSGpasv(FILE* fd, char* iprecv, char* portrecv) {
    char* msg;
    char ip1[10], ip2[10], ip3[10], ip4[10], ip5[10], ip6[10];
    size_t rbytes = 0;

    getline(&msg, &rbytes, fd);
    //printf("< %s", msg);

    sscanf(msg, IP1_REGEX, ip1);
    sscanf(msg, IP2_REGEX, ip2);
    sscanf(msg, IP3_REGEX, ip3);
    sscanf(msg, IP4_REGEX, ip4);
    sscanf(msg, IP5_REGEX, ip5);
    sscanf(msg, IP6_REGEX, ip6);

    sprintf(iprecv,"%s.%s.%s.%s", ip1, ip2, ip3, ip4);
    sprintf(portrecv, "%d", (atoi(ip5) * 256) + atoi(ip6));

    //printf("ip | port : %s | %s", iprecv,portrecv);

    return 0;
}

int parseInput(char* input, info* h_info) {

    strcpy(h_info->pass, "");

    sscanf(input, HOST_REGEX, h_info->host);
    sscanf(input, USER_REGEX, h_info->user);
    sscanf(input, PASSWORD_REGEX, h_info->pass);
    sscanf(input, PATH_REGEX, h_info->path);
    strcpy(h_info->file_name, strrchr(h_info->path, '/') + 1);  

    if (strcmp(h_info->pass, "") == 0){
        strcpy(h_info->user, "anonymous");
        strcpy(h_info->pass, "pass");
        sscanf(input, HOST2_REGEX, h_info->host);
    }
    
    /*
    printf("host: %s\n", h_info->host);
    printf("user: %s\n", h_info->user);
    printf("pass: %s\n", h_info->pass);
    printf("path: %s\n", h_info->path);
    printf("filename: %s\n", h_info->file_name);
    */

    return 0;
}

int saveToFile(int fd, char* filename) {
    int file;   
    int rbytes;
    char buf[512];
    if ((file = open(filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        printf("Cannot open/create file\n");
        return -1;
    }

    while((rbytes = read(fd, buf, sizeof(buf))) != 0){
        if (rbytes > 0) {
            write(file, buf, rbytes);
            //printf("%s", buf);
        }
    }   
    close(file);

    return 0;
}