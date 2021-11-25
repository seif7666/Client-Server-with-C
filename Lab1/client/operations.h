#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/stat.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fcntl.h>



#define GET_REQUEST 0
#define POST_REQUEST 1
#define HOSTNAME_SIZE 30
#define DEFAULT_PORT 80
#define GET_REQUEST_STR "client_get"
#define POST_REQUEST_STR "client_post"

typedef struct command{
    char request;
    char hostname[HOSTNAME_SIZE];
    char filePath[HOSTNAME_SIZE];
    char port[5];
    struct command *next;
}Command;

void read_input_file(Command* commands, char* filename);
void init_command(Command *command,int request,char*filePath,char* hostname, char* port);
void add_command(Command* commands, Command* command);
void print_command(Command* command);
void manage_command(Command command, struct addrinfo hints);

void manageGetRequest(Command command, int socket);
void managePostRequest(Command command, int socket);

void setFilePathAndDirectory(char* fileNameBuffer, const char* request, char* fileName, char* hostName);

#endif