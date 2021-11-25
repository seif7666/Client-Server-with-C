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
        printf("Get add failed!\n");
        return;
    }
    int sock= socket(res->ai_family, res->ai_socktype,res->ai_protocol);
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
    memset(filePath,0,50);
    strcat(filePath, "GET/");
    strcat(filePath, fileName);
    printf("%s\n",filePath);
    char buffer[BUFSIZ];
    int bytes;
    FILE *fp= fopen(filePath, "w");

    bytes= recv(socket,buffer,sizeof(buffer), 0 );
    char * data= strstr(buffer, "\r\n\r\n");
    *data= 0;
    data+=4;
    printf("\n%s\n",buffer);
    fprintf(fp, "%s",data);
    do{
        bytes= recv(socket,buffer,sizeof(buffer), 0 );
        fprintf(fp,"%s",buffer);
    }while(bytes>0);
    fclose(fp);
}
void manageGetRequest(Command command, int socket){
    char buffer[2048];
    sprintf(buffer, "GET /%s HTTP/1.1\r\n", command.filePath);
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", command.hostname, command.port);
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    // sprintf(buffer + strlen(buffer), "User-Agent: honpwc web_get 1.0\r\n");
    sprintf(buffer + strlen(buffer), "\r\n");
    send(socket, buffer, strlen(buffer), 0);
    printf("Sent Headers:\n%s", buffer);
    receiveRequestFile(socket , command.filePath, command.hostname);
}

void managePostRequest(Command command, int socket){

}