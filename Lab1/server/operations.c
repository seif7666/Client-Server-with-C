#include "operations.h"

int clientSocket;
int exitWithMessage(char* message){
    printf("%s\n",message);
    exit(-1);
}

int manageClient(int cs){
    clientSocket= cs;

    char buffer[400000];

    ssize_t numBytesReceived= recv(clientSocket, buffer, BUFSIZ, 0);

    char sendingBuffer[400000];
    int length=manageSendingData(buffer, numBytesReceived, sendingBuffer);
    // send(clientSocket,buffer,BUFSIZ,0);
    return numBytesReceived;
}

int manageSendingData(char* receivedBuffer, int bytesReceived, char* sendingBuffer){
    if(strstr(receivedBuffer, "GET") != 0)
        return handleGetRequest(receivedBuffer,bytesReceived,sendingBuffer);
    else if(strstr(receivedBuffer,"POST") != 0)
        printf("Post Request!\n");    
}

int handleGetRequest(char* receivedBuffer, int numBytesReceived, char* sendingBuffer){
    //It starts with a GET, so we skip 4 characters
    char *ptr= receivedBuffer+5; //Beginning of file name.
    char* nameEnd= ptr;
    printf("///%c///",*ptr);
    while(*nameEnd != ' ')
        nameEnd++;
    *nameEnd='\0';
    printf("String is %s\n",ptr);
    FILE *fileptr=fopen(ptr, "r");
    if(fileptr == NULL){
        send_404();
        return 0;
    }
    char filetype[20];
    if(strstr(ptr, "html") != 0)
        sprintf(filetype, "text/html");
    else if(strstr(ptr,"txt") != 0)
        sprintf(filetype, "text/plain");
    else if(strstr(ptr, "png"))
        sprintf(filetype, "image/png");
    else { //Images only
        sprintf(filetype, "image/jpeg");
    }
        

    int fileSize= readFile(fileptr,sendingBuffer);
    printf("Returned from readFile\n");
    char buffer[100];
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(clientSocket, buffer, strlen(buffer),0);

    sprintf(buffer, "Connection: keep-alive\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %u\r\n", fileSize);
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n",filetype);
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    
    int totalSize= strlen(sendingBuffer);
    printf("Total Size is %d\n",totalSize);
    ssize_t sent= 0;
    while(sent < totalSize){
        ssize_t x=send(clientSocket, sendingBuffer+sent, BUFSIZ, 0);
        if(x < 0)break;
        sent +=x;
        printf("Sent %d bytes Left it is %d\n",sent,strlen(sendingBuffer+sent));
    }
    // send(clientSocket, sendingBuffer, strlen(sendingBuffer), 0);
}

int readFile(FILE* fileptr, char* buffer){
    //Now we start reading.
    char fileBuffer[50];
    int bytesRead= 0;
    while(fgets(fileBuffer, 50,fileptr)){
        int length= strlen(fileBuffer);
        bytesRead += length;
        strcat(buffer,fileBuffer);
    }
    // printf("File:\n\n\n%s\n\n",buffer);
    fclose(fileptr);
    return bytesRead;
}


void send_404(){
    char *message = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 9\r\n\r\nNot Found";
    send(clientSocket, message, strlen(message), 0);
}