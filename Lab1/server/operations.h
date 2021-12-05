#ifndef OPERATIONS_H
#define OPERATIONS_H
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>


#define MAXPENDING 5


int manageClient(int clientSocket);
int manageSendingData(char* receivedBuffer, int numBytesReceived, int clientSocket);
void handleGetRequest(char* receivedBuffer, int numBytesReceived, int clientSocket);
void handlePostRequest(char* buffer, int bytesReceived,int clientSocket);
int readFile(FILE* fileName, char* buffer);
void setContentType(char* fileName, char* contentName);
void sendHttpOK(int fileSize,char *contentType,int clientSocket);
void send_404(int clientSocket);

#endif