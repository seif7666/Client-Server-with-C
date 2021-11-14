/**
 * Create socket
 * Establish connection with a server
 * Communicate
 * close connection
 */
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int exitWithMessage(char* message){
    printf("Entered here!");
    printf("%s\n",message);
    exit(-1);
}

