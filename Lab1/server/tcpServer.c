#include "tcpserver.h"
#define TOTAL_WAITING_TIME 15
#define MINIMUM_WAITING_TIME 2

#define STDIN 0


int clients_connected; //To Keep track of total clients having persistent connections.
pthread_mutex_t lock; // To avoid race condition on clients_connected


char terminateProgram= 0;
const char* SERVER_IP= "127.0.0.1";

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


    clients_connected= 0;

    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    
    fd_set master;
    FD_ZERO(&master);
    FD_SET(sock, &master);
    FD_SET(STDIN, &master);
    

    while(1){
        fd_set reads= master;
        select(sock+1, &reads,NULL,NULL,NULL);
        if(FD_ISSET(sock, &reads)){
            addrlen= sizeof(remoteaddr);
            int newClientSocket= accept(sock, (struct sockaddr *) &remoteaddr, &addrlen);
            if(newClientSocket == -1) exitWithMessage("Accept Failed!");
            char* remoteIP[INET6_ADDRSTRLEN];
            printf("New Connection established with %s\n",inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*) &remoteaddr),remoteIP,INET6_ADDRSTRLEN));
            clients_connected ++;

            pthread_t newThread;
            ClientThreadArgs arguments;
            arguments.clientSocket= newClientSocket;
            
            pthread_create(&newThread, NULL,&manageThreadedClient, (void*)&arguments );
        }
        else //Stdin
            break;
    }
    terminateProgram= 1;
    close(sock);
    printf("Terminating...\n");
    
    return 0;
}

void manageThreadedClient(void* arguments){
    ClientThreadArgs * args= (ClientThreadArgs*) arguments;
    
    struct timeval tv;
    tv.tv_usec = 500000;

    while(!terminateProgram){

        tv.tv_sec = TOTAL_WAITING_TIME / clients_connected;
        if (tv.tv_sec < MINIMUM_WAITING_TIME)
            tv.tv_sec= MINIMUM_WAITING_TIME;
        printf("New Timeout is %d\n",tv.tv_sec);

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(args->clientSocket, &readfds);

        select(args->clientSocket + 1, &readfds, NULL, NULL, &tv);
        
        if (FD_ISSET(args->clientSocket, &readfds)){
            int isConnected=manageClient(args->clientSocket);
            if(!isConnected){
                printf("Socket %d disconnected\n",args->clientSocket);
                break;
            }
        }        
        else{
            printf("Client {%d} Timed Out\n",args->clientSocket);
            break;
        }
    }
    close(args->clientSocket);
    decreaseClients();
}


void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int exitWithMessage(char* message){
    printf("%s\n",message);
    terminateProgram= 1;
    exit(-1);
}

void decreaseClients(){
    pthread_mutex_lock(&lock);
    clients_connected--;
    printf("Total number of connected clients now is : %d\n",clients_connected);
    pthread_mutex_unlock(&lock);
}