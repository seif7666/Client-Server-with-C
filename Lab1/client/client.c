#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc, char** argv){
    if(argc != 3){
        printf("Enter hostname and port!\n");
        return -1;
    }
    char* hostName= argv[1];
    char* port= argv[2];
    char ipstr[INET6_ADDRSTRLEN];

    printf("%s:%s\n",hostName,port);
 
    struct addrinfo hints , *res, *p;
    memset(&hints, 0,sizeof(hints));
    hints.ai_family= AF_UNSPEC;
    hints.ai_socktype= SOCK_STREAM;

    if(getaddrinfo(hostName, NULL, &hints, &res)){
        printf("Get Addrinfo failed\n");
        return -1;
    }
    for(p= res; p!=NULL; p= p->ai_next){
        void *addr;
        char* ipver;

        if(p->ai_family == AF_INET){
            struct sockaddr_in *ipv4= (struct sockaddr_in *)p->ai_addr;
            addr=  &(ipv4->sin_addr);
            ipver= "IPV4";
        }
        else{
            struct sockaddr_in6 *ipv6= (struct sockaddr_in6 *)p->ai_addr;
            addr=  &(ipv6->sin6_addr);
            ipver= "IPV6";
        }
        inet_ntop(p->ai_family,addr,ipstr,sizeof(ipstr));
        printf(" %s: %s\n",ipver,ipstr);
    }
    freeaddrinfo(res);

    return 0;
}