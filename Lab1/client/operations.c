#include "operations.h"


void read_input_file(Command* commands, char* filename){
    FILE *fp= fopen(filename, "r");
    char line[100];
    while(fgets(line,100,fp)){
      
        char* method= strtok(line, " ");
        char* filePath= strtok(NULL," ");
        char* hostname= strtok(NULL, " ");
        char* port= strtok(NULL, " ");
        int portNumber = port != NULL ? atoi(port): DEFAULT_PORT;

        Command *command = (Command*)malloc(sizeof(Command));
        init_command(command, strcmp(GET_REQUEST_STR,method) ? POST_REQUEST : GET_REQUEST , filePath,hostname, portNumber);
        add_command(commands, command);
    }
}

void init_command(Command *command,int request,char* filePath,char* hostname, int port){
    command -> request= request;
    strncpy(command->hostname, hostname,strlen(hostname));
    strncpy(command->filePath, filePath,strlen(filePath));
    command->port= port;
    command->next= NULL;
    print_command(command);
}
void add_command(Command* commands, Command* command){
    Command *temp = commands;
    while(temp->next != NULL)
        temp= temp->next;
    temp->next= command;
}
void print_command(Command* command){
    printf("Request: %d , FilePath: %s, Hostname: %s, Port: %d\n",command->request, command->filePath,command->hostname,command->port);
}
