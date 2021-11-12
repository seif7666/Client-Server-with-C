#include "operations.h"


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

    while(1){
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLen= sizeof(clientAddress);
        
        printf("Waiting for connection...\n");
        int clientSocket= accept(sock, (struct sockaddr *) &clientAddress, &clientAddressLen);
        if(clientSocket < 0){
            // WSAGetLastError();
            printf("Error message %d\n",clientSocket);
            exitWithMessage("Accept failed!");
        }
        
        char clientName[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET ,&clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) != NULL)
            printf("Handling client %s/%d\n\n", clientName, ntohs(clientAddress.sin_port));
        else
            printf("Unable to get client address\n");

        manageClient(clientSocket);
    }
    close(sock);
    
    return 0;
}