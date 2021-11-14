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

int main(int argc, char *argv[]){
    if(argc < 3 || argc >4){
        printf("Invalid Arguments\n");
        exit(-1);
    }

    char* servIP= argv[1];
    char* echoString= argv[2];

    in_port_t servPort= (argc ==4) ? atoi(argv[3]) : 7;//Port for echo

    printf("Creating socket...\n");
    int sock= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0)
        exitWithMessage("Failed to create socket!");

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;

    int rtnVal= inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if(rtnVal = 0)
        exitWithMessage("inet_pton failed, invalid address");
    else if(rtnVal <0)
        exitWithMessage("iton failed");
    
    servAddr.sin_port= htons(servPort);

    printf("Establishing connection...\n");

    if(connect(sock,(struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        exitWithMessage("Connection Failed!");
    
    printf("Sending message ...\n");
    size_t echoStringLen= strlen(echoString);
    ssize_t numBytes= send(sock,echoString, echoStringLen, 0);
    if(numBytes < 0)
        exitWithMessage("Failed to send!");
     
    printf("Waiting to receive...\n");
    unsigned int bytesReceived= 0;
    printf("Received: ");
    while(bytesReceived < echoStringLen){
        char buffer[BUFSIZ];
        numBytes= recv(sock, buffer, BUFSIZ-1, 0);
        if(numBytes <= 0)
            exitWithMessage("Connection closed!");
        bytesReceived += numBytes;
        buffer[numBytes]= '\0';
        printf("%s\n",buffer);
    }
    printf("Closing ....\n");
    close(sock);

    return 0;
}