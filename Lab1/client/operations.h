#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


#define GET_REQUEST 0
#define POST_REQUEST 1
#define HOSTNAME_SIZE 30
#define DEFAULT_PORT 80
#define GET_REQUEST_STR "client_get"

typedef struct command{
    char request;
    char hostname[HOSTNAME_SIZE];
    char filePath[HOSTNAME_SIZE];
    int port;
    struct command *next;
}Command;

void read_input_file(Command* commands, char* filename);
void init_command(Command *command,int request,char*filePath,char* hostname, int port);
void add_command(Command* commands, Command* command);
void print_command(Command* command);

#endif