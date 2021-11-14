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

    freeaddrinfo(res);
    close(sock);
    
}