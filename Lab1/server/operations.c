#include "operations.h"

#define MAX_FILE_SIZE 400000


int manageClient(int cs){
    char buffer[MAX_FILE_SIZE];
    memset(buffer,0,sizeof(buffer));
    ssize_t numBytesReceived= recv(cs, buffer, BUFSIZ, 0);
    if(numBytesReceived > 0){
        // buffer[numBytesReceived]= '\0';
        // printf("Received: %s\n",buffer);
        char sendingBuffer[MAX_FILE_SIZE];
        int length=manageSendingData(buffer, numBytesReceived ,cs);
    }
    return numBytesReceived;
}

int manageSendingData(char* receivedBuffer, int bytesReceived , int clientSocket){
    if(strstr(receivedBuffer, "GET") != 0)
         handleGetRequest(receivedBuffer,bytesReceived , clientSocket);
    else if(strstr(receivedBuffer,"POST") != 0)
        handlePostRequest(receivedBuffer, bytesReceived, clientSocket);
    return 0;
}

void handlePostRequest(char* buffer, int bytesReceived, int clientSocket){
    //Look for content-type and size
    char* fileName= strstr(buffer,"/")+1;

    char* contentTypePtr= strstr(buffer, "Content-Type") + 14;
    char* typeTerminator= strstr(contentTypePtr, "\r");

    char* contentLengthPtr= strstr(buffer,"Content-Length:") + 15;
    char* lengthTerminator= strstr(contentLengthPtr,"\r");

    char *data= strstr(buffer,"\r\n\r\n");
    *data= 0;
    data+=4;

    printf("--------------------------POST REQUEST-----------------------\n%s\n----------------------- ",buffer);
    *lengthTerminator=0;
    *typeTerminator=0;

    // *fileTerminator=0;
    int contentLength= atoi(contentLengthPtr);
    printf("Content is <%s>\n",contentTypePtr);
    printf("Length is %d\n",atoi(contentLengthPtr));
    fileName= strtok(fileName, " ");
    printf("File: %s\n",fileName);
    // printf("Data is \"\"\"%s\"\"\"\n",data);
    FILE *fp= fopen(fileName,"w");
    int received= strlen(data);
    fwrite(data,1,strlen(data),fp);
    while(received < contentLength){
        memset(buffer, 0, sizeof(buffer));
        int bytes=recv(clientSocket, buffer,sizeof(buffer), 0);
        if(bytes > 0){
            int writeLength= bytes > (contentLength-received) ? contentLength-received : bytes;
            fwrite(buffer, 1, writeLength,fp);
            received += bytes;
        }
        else
            break;
    }
    fclose(fp);
    sendHttpOK(0,0,clientSocket);

} 

void handleGetRequest(char* receivedBuffer, int numBytesReceived , int clientSocket){
    //It starts with a GET, so we skip 4 characters
    printf("------------------------GET REQUEST-----------------------\n%s\n-------------------------\n",receivedBuffer);
    char *ptr= receivedBuffer+5; //Beginning of file name.
    char* nameEnd= ptr;
    // printf("///%c///",*ptr);
    while(*nameEnd != ' ')
        nameEnd++;
    *nameEnd='\0';

    FILE *fileptr=fopen(ptr, "rb");
    printf("-------------GET RESPONSE--------\n");
    if(fileptr == NULL){
        send_404(clientSocket);
        return;
    }
    char filetype[20];
    setContentType(ptr,filetype);

    fseek(fileptr, 0,SEEK_END);
    int fileSize= ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    sendHttpOK(fileSize,filetype, clientSocket);
    
    char sendingBuffer[fileSize];
    readFile(fileptr,sendingBuffer);
    printf("%s\n-------------------------------\n",sendingBuffer);
    ssize_t sent= 0;
    int totalSize=fileSize;
    while(totalSize >0){
        ssize_t x=send(clientSocket, sendingBuffer+sent, totalSize, 0);
        if(x < 0)break;
        totalSize -=x;
        printf("Left %d bytes \n",totalSize);
    }
}

int readFile(FILE* fileptr, char* buffer){
    //Now we start reading.
    memset(buffer,0,sizeof(buffer));
    int bytesRead= 0;
    int length;
    while((length=fread(buffer+bytesRead, 1,sizeof(buffer+bytesRead),fileptr))){
        bytesRead += length;
    }
    // printf("File:\n\n\n%s\n\n",buffer);
    fclose(fileptr);
    return bytesRead;
}


void send_404(int clientSocket){
    char *message = "HTTP/1.1 404 Not Found\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 9\r\n\r\nNot Found";
    printf("%s\n-------------------------------\n",message);
    send(clientSocket, message, strlen(message), 0);
}

void sendHttpOK(int fileSize,char *contentType, int clientSocket){
    char buffer[100];
    memset(buffer, 0, 100);
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");

    if(contentType != 0){
    sprintf(buffer+strlen(buffer), "Connection: keep-alive\r\n");
    // send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer+strlen(buffer), "Content-Length: %u\r\n", fileSize);
    // send(clientSocket, buffer+, strlen(buffer), 0);

    sprintf(buffer+strlen(buffer), "Content-Type: %s\r\n",contentType);
    }
    sprintf(buffer+strlen(buffer), "\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

}

void setContentType(char* fileName, char* contentName){
    if(strstr(fileName, "html") != 0)
        sprintf(contentName, "text/html");
    else if(strstr(fileName,"txt") != 0)
        sprintf(contentName, "text/plain");
    else if(strstr(fileName, "png"))
        sprintf(contentName, "image/png");
    else { //Images only
        sprintf(contentName, "image/jpeg");
    }
}