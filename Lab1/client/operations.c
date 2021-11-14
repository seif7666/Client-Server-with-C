#include "operations.h"


void read_input_file(Command* commands, char* filename){
    FILE *fp= fopen(filename, "r");
    char line[100];
    while(fgets(line,100,fp)){
      
        char* method= strtok(line, " ");
        char* filePath= strtok(NULL," ");
        char* hostname= strtok(NULL, " ");
        char* port= strtok(NULL, " ");
        if (port != NULL)
            port[strlen(port)-1]= '\0';
        Command *command = (Command*)malloc(sizeof(Command));
        init_command(command, strcmp(GET_REQUEST_STR,method) ? POST_REQUEST : GET_REQUEST , filePath,hostname, port ==NULL ? "80":port);
        add_command(commands, command);
    }
}

void init_command(Command *command,int request,char* filePath,char* hostname, char* port){
    command -> request= request;
    strncpy(command->hostname, hostname,strlen(hostname));
    strncpy(command->filePath, filePath,strlen(filePath));
    strncpy(command->port, port,strlen(port));
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
    printf("Request: %d , FilePath: %s, Hostname: %s, Port: %s\n",command->request, command->filePath,command->hostname,command->port);
}
