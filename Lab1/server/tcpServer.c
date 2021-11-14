#include "operations.h"
#include <sys/time.h>

void *get_in_addr(struct sockaddr *sa){
 if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
 }

 return &(((struct sockaddr_in6*)sa)->sin6_addr);
 }

const char* SERVER_IP= "192.168.1.110";

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Invalid Arguments\n");
        exit(-1);
    }

    in_port_t servPort= atoi(argv[1]);

    printf("Creating socket...\n");
    int sock= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0)
        exitWithMessage("Failed to create socket!");

   
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr= htonl(INADDR_ANY);
    servAddr.sin_port= htons(servPort);

    int rtnVal= inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr.s_addr);


    printf("Binding...\n");
    if(bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        exitWithMessage("Error Binding!");
    
    if(listen(sock, MAXPENDING ) < 0)
        exitWithMessage("Failed to Listen");
    printf("Listening ...\n");


    fd_set master, read_fds;
    int fdmax= sock, newfd;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(sock,&master);


    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
                    






    while(1){
        read_fds= master;
        printf("Before Select\n");
        if(select(fdmax+1, &read_fds,NULL,NULL, /*Timout*/NULL) ==-1){
            exitWithMessage("Select Failed!");
        }
        printf("Returned From Select!\n");
        for(int i= 0; i<= fdmax ; i++){
            if(FD_ISSET(i,&read_fds)){
                if(i==sock){
                    addrlen= sizeof(remoteaddr);
                    newfd= accept(sock, (struct sockaddr *) &remoteaddr, &addrlen);
                    if(newfd == -1) exitWithMessage("Accept Failed!");

                    FD_SET(newfd, &master);
                    if(newfd > fdmax)
                        fdmax= newfd;
                    char* remoteIP[INET6_ADDRSTRLEN];
                    printf("New Connection established with %s\n",inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*) &remoteaddr),remoteIP,INET6_ADDRSTRLEN));
                }//i==sock
                else{
                    int isConnected=manageClient(i);
                    if(!isConnected){
                        // printf("Socket %d disconnected\n",i);
                        close(i);
                        FD_CLR(i, &master);
                    }
                }
            }
        }
    }
    close(sock);
    
    return 0;
}