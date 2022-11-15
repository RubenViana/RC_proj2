#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

typedef struct info
{
  char user[128];       //< User used for Login
  char pass[128];       //< Password used for Login
  char host[256];  //< Host name in URL
  char path[240];       //< Path to the file
  char file_name[128];  //< Name of the File
  char host_name[128];  //< Host Name from gethostbyname()
  char ip[128];         //< IP Adress from gethostbyname()

} info;

int main(int argc, char **argv);        //return 0 on success, -1 otherwise

int getIp(char* hostName, info* h_info);     //return 0 on success, -1 otherwise

int parseInput(char* input, info* h_info);      //return 0 on success, -1 otherwise

int sendCMD(int sockfd, char* cmd);          //return 0 on success, -1 otherwise

int readResp(int sockfd);    //return 0 on success, -1 otherwise

int readRespPasv(int sockfd);


#endif