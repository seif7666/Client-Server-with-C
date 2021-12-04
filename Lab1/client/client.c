#include "operations.h"


void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
}

int main(int argc, char** argv){
    Command commands;
    commands.next= NULL;
    read_input_file(&commands, "input.txt");


    struct addrinfo hints ;

    memset(&hints, 0,sizeof(hints));
    hints.ai_family= AF_INET; 
    hints.ai_socktype= SOCK_STREAM;


    Command* temp = commands.next;
    while(temp != NULL){
        manage_command(*temp,hints);
        temp= temp->next;
    }

    return 0;
}

void manage_command(Command command, struct addrinfo hints){
    struct addrinfo *res;
    if(getaddrinfo(command.hostname, command.port, &hints, &res)<0){
        printf("Get addr failed for hostname {%s}!\n",command.hostname);
        return;
    }
    int sock= socket(res->ai_family, res->ai_socktype,res->ai_protocol);
    // fcntl(sock,F_SETFL,O_NONBLOCK);
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    if(sock == -1){
        printf("Socket Failed!\n");
        return;
    }
    if(connect(sock,res->ai_addr, res->ai_addrlen)<0){
        printf("Failed to connect!\n");
        return;
    }
    char s[INET_ADDRSTRLEN];
    inet_ntop(res->ai_family, get_in_addr((struct sockaddr *)res->ai_addr),s, sizeof (s));
    printf("Connected to %s\n",s);
    switch(command.request){
        case GET_REQUEST: manageGetRequest(command,sock);break;
        case POST_REQUEST: managePostRequest(command,sock);break;
    }
    freeaddrinfo(res);
    close(sock);
}
void receiveRequestFile(int socket , char* fileName , char*hostname){

    char filePath[50];
    setFilePathAndDirectory(filePath,GET_REQUEST_STR,fileName, hostname);
    printf("%s\n",filePath);
    FILE *fp= fopen(filePath, "w");
    if(fp == NULL)
        printf("NULL\n");

    char buffer[BUFSIZ];
    memset(buffer, 0,BUFSIZ);
    int bytes;

    bytes= recv(socket,buffer,sizeof(buffer), 0 );
    if(!strstr(buffer ,"200 OK")){
        printf("--------NotFound----\n%s\n-------------\n",buffer);
        return;
    }
    char * data= strstr(buffer, "\r\n\r\n");
    if(data != NULL){
        *data= 0;
        data+=4;
        printf("-----Get Response-----------\n%s\n--------------\n",buffer);
        fprintf(fp, "%s",data);
    }
    do{
        memset(buffer, 0,BUFSIZ);
        bytes= recv(socket,buffer,sizeof(buffer), 0 );
        printf("Bytes Received: %d\n",bytes);
        fprintf(fp,"%s",buffer);
    }while(bytes>0);
    fclose(fp);
}
void manageGetRequest(Command command, int socket){
    char buffer[2048];
    sprintf(buffer, "GET /%s HTTP/1.1\r\n", command.filePath);
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", command.hostname, command.port);
    sprintf(buffer + strlen(buffer), "Connection: keep-alive\r\n");
    sprintf(buffer + strlen(buffer), "\r\n");
    send(socket, buffer, strlen(buffer), 0);
    printf("Sent Headers:\n%s", buffer);
    receiveRequestFile(socket , command.filePath, command.hostname);
}


/**
 * We need to specify length and type of posted file
 */
void addTypeAndLengthToBuffer(char* buffer, int fileSize, char* fileName){
    char contentName[30];
    setContentType(fileName, contentName);
    sprintf(buffer + strlen(buffer), "Content-Type: %s\r\n",contentName);
    //Now we get the total length of the file.
    sprintf(buffer+strlen(buffer), "Content-Length: %d\r\n",fileSize);
}


void sendFileToSocket(FILE* fptr, int size, int socket){
    char sendBuffer[BUFSIZ];
    int sent= 0;
    while(sent <= size){
        memset(sendBuffer,0,sizeof(sendBuffer));
        char *bufptr= sendBuffer;
        int length= 0;
        while((bufptr < sendBuffer+ BUFSIZ) &&(!feof(fptr))){
            *bufptr= fgetc(fptr);
            bufptr++;
            length++;
        }
        // printf("%d\n----------------\n",length);
        int x= send(socket,sendBuffer, length, 0);
        printf("Sent= %d\n",sent+x);
        if(x > 0)
            sent +=x;
        else{
            break; //Connection was closed.
        }
    }
    fclose(fptr);
}

void managePostRequest(Command command, int socket){
    FILE* fileptr= fopen(command.filePath, "r");
    if(fileptr == NULL){
        printf("Error!File: \"%s\" was not found!\n",command.filePath);
        return;
    }
    fseek(fileptr, 0,SEEK_END);
    int fileSize= ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    char buffer[2048];
    sprintf(buffer, "POST /%s HTTP/1.1\r\n",command.filePath);
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", command.hostname, command.port);
    sprintf(buffer + strlen(buffer), "Connection: keep-alive\r\n");
    addTypeAndLengthToBuffer(buffer, fileSize,command.filePath);
    sprintf(buffer + strlen(buffer), "\r\n");
    send(socket, buffer, strlen(buffer), 0);
    printf("Sent Headers:\n%s", buffer);
    sendFileToSocket(fileptr,fileSize,socket);

    char receiveBuffer[BUFSIZ];
    memset(receiveBuffer, 0, BUFSIZ);
    int bytes_received= recv(socket, receiveBuffer, BUFSIZ,0);
    printf("///////////////\n%s\n/////////",receiveBuffer); 
}