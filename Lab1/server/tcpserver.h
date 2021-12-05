#include "operations.h"
#include <sys/time.h>
#include <pthread.h>

typedef struct clientArgs{
    fd_set* master;
    int clientSocket;
}ClientThreadArgs;

void *get_in_addr(struct sockaddr *sa);
void manageThreadedClient(ClientThreadArgs* args);