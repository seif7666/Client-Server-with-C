#include "operations.h"

#define MAX_FILE_SIZE 400000

int clientSocket;
int exitWithMessage(char* message){
    printf("%s\n",message);
    exit(-1);
}

int manageClient(int cs){
    clientSocket= cs;
    char buffer[MAX_FILE_SIZE];
    ssize_t numBytesReceived= recv(clientSocket, buffer, BUFSIZ, 0);
    buffer[numBytesReceived]= '\0';
    printf("Received: %s\n",buffer);
    char sendingBuffer[MAX_FILE_SIZE];
    int length=manageSendingData(buffer, numBytesReceived);
    return numBytesReceived;
}

int manageSendingData(char* receivedBuffer, int bytesReceived){
    if(strstr(receivedBuffer, "GET") != 0)
         handleGetRequest(receivedBuffer,bytesReceived);
    else if(strstr(receivedBuffer,"POST") != 0)
        handlePostRequest(receivedBuffer, bytesReceived);
    return 0;
}

void handlePostRequest(char* buffer, int bytesReceived){
    //Look for content-type and size
    char* contentTypePtr= strstr(buffer, "Content-Type") + 14;
    char* typeTerminator= strstr(contentTypePtr, "\r");

    char* contentLengthPtr= strstr(buffer,"Content-Length:") + 15;
    char* lengthTerminator= strstr(contentLengthPtr,"\r");

    char *data= strstr(buffer,"\r\n\r\n");
    *data= 0;
    data+=4;

    *lengthTerminator=0;
    *typeTerminator=0;
    printf("Content is <%s>\n",contentTypePtr);
    printf("Length is %d\n",atoi(contentLengthPtr));
    printf("Data is \"\"\"%s\"\"\"\n",data);

    sendHttpOK(0,0);

    close(clientSocket);

} 

void handleGetRequest(char* receivedBuffer, int numBytesReceived){
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
    int totalSize=fileSize;
    while(totalSize >0){
        ssize_t x=send(clientSocket, sendingBuffer+sent, totalSize, 0);
        if(x < 0)break;
        totalSize -=x;
        printf("Left %d bytes \n",totalSize);
    }
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

void sendHttpOK(int fileSize,char *contentType){
    char buffer[100];
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(clientSocket, buffer, strlen(buffer),0);



    if(contentType != 0){
    sprintf(buffer, "Connection: keep-alive\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %u\r\n", fileSize);
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n",contentType);
    send(clientSocket, buffer, strlen(buffer), 0);
    }

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