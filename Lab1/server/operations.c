#include "operations.h"

#define MAX_FILE_SIZE 400000

int clientSocket;
int exitWithMessage(char* message){
    printf("%s\n",message);
    exit(-1);
}

void manageClient(int cs){
    clientSocket= cs;
    char buffer[MAX_FILE_SIZE];
    ssize_t numBytesReceived= recv(clientSocket, buffer, BUFSIZ, 0);
    buffer[numBytesReceived]= '\0';
    printf("Received: %s\n",buffer);
    char sendingBuffer[MAX_FILE_SIZE];
    int length=manageSendingData(buffer, numBytesReceived);
}

int manageSendingData(char* receivedBuffer, int bytesReceived){
    if(strstr(receivedBuffer, "GET") != 0)
        return handleGetRequest(receivedBuffer,bytesReceived);
    else if(strstr(receivedBuffer,"POST") != 0)
        printf("Post Request!\n");    
}

int handleGetRequest(char* receivedBuffer, int numBytesReceived){
    //It starts with a GET, so we skip 4 characters
    char *ptr= receivedBuffer+5; //Beginning of file name.
    char* nameEnd= ptr;
    printf("///%c///",*ptr);
    while(*nameEnd != ' ')
        nameEnd++;
    *nameEnd='\0';
    printf("String is %s\n",ptr);
    FILE *fileptr=fopen(ptr, "rb");
    if(fileptr == NULL){
        send_404();
        return 0;
    }
    char filetype[20];
    setContentType(ptr,filetype);

    fseek(fileptr, 0,SEEK_END);
    int fileSize= ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    sendHttpOK(fileSize,filetype);
    
    char sendingBuffer[fileSize];
    readFile(fileptr,sendingBuffer);
    printf("Total Size is %d\n",fileSize);
    ssize_t sent= 0;
    while(sent < fileSize){
        ssize_t x=send(clientSocket, sendingBuffer+sent, BUFSIZ, 0);
        if(x < 0)break;
        sent +=x;
        printf("Sent %d bytes \n",sent);
    }
    close(clientSocket);
    // send(clientSocket, sendingBuffer, strlen(sendingBuffer), 0);
}

int readFile(FILE* fileptr, char* buffer){
    //Now we start reading.
    memset(buffer,0,sizeof(buffer));
    char fileBuffer[50];
    int bytesRead= 0;
    while(fgets(fileBuffer, 50,fileptr)){
        int length= strlen(fileBuffer);
        bytesRead += length;
        strcat(buffer,fileBuffer);
    }
    printf("File:\n\n\n%s\n\n",buffer);
    return 0;
}


void send_404(){
    char *message = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 9\r\n\r\nNot Found";
    send(clientSocket, message, strlen(message), 0);
}

void sendHttpOK(int fileSize,char *contentType){
    char buffer[100];
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(clientSocket, buffer, strlen(buffer),0);

    sprintf(buffer, "Connection: keep-alive\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %u\r\n", fileSize);
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n",contentType);
    send(clientSocket, buffer, strlen(buffer), 0);

        // sprintf(buffer, "Transfer-Encoding: chunked\r\n");
    // send(clientSocket, buffer, strlen(buffer), 0);


    sprintf(buffer, "\r\n");
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