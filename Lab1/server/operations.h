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

int exitWithMessage(char* message);
int manageClient(int clientSocket);
int manageSendingData(char* receivedBuffer, int numBytesReceived, char* sendingBuffer);
int handleGetRequest(char* receivedBuffer, int numBytesReceived, char* sendingBuffer);
int readFile(FILE* fileName, char* buffer);
void send_404();

#endif