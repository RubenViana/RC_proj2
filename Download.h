#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define HOST_REGEX "%*[^/]//%*[^@]@%[^/]"
#define HOST2_REGEX "%*[^/]//%[^/]"
#define PATH_REGEX "%*[^/]//%*[^/]%s"
#define USER_REGEX "%*[^/]//%[^:]"
#define PASSWORD_REGEX "%*[^/]//%*[^:]:%[^@]"

#define IP1_REGEX "%*[^(](%[^,]"
#define IP2_REGEX "%*[^(](%*[^,],%[^,]"
#define IP3_REGEX "%*[^(](%*[^,],%*[^,],%[^,]"
#define IP4_REGEX "%*[^(](%*[^,],%*[^,],%*[^,],%[^,]"
#define IP5_REGEX "%*[^(](%*[^,],%*[^,],%*[^,],%*[^,],%[^,]"
#define IP6_REGEX "%*[^(](%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^)]"

#define SERVER_PORT 21

#include <stdio.h>

typedef struct info
{
  char user[128];       //< User used for Login
  char pass[128];       //< Password used for Login
  char host[256];       //< Host name in URL
  char path[256];       //< Path to the file
  char file_name[128];  //< Name of the File
  char host_name[128];  //< Host Name from gethostbyname()
  char ip[128];         //< IP Adress from gethostbyname()

} info;

int main(int argc, char **argv);        //return 0 on success, -1 otherwise

int openConnection(int* fd, char* ip, int port);      //return 0 on success, -1 otherwise

int closeConnection(int fd);      //return 0 on success, -1 otherwise

int getIp(info* h_info);     //return 0 on success, -1 otherwise

int parseInput(char* input, info* h_info);      //return 0 on success, -1 otherwise

int sendCMD(int sockfd, char* cmd);          //return 0 on success, -1 otherwise

int recvMSG(FILE* fd);    //return 0 on success, -1 otherwise

int recvMSGpasv(FILE* fd, char* iprecv, char* portrecv);    //return 0 on success, -1 otherwise

int saveToFile(int fd, char* filename);     //return 0 on success, -1 otherwise


#endif