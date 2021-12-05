#include "operations.h"
#include <sys/time.h>
#include <pthread.h>

typedef struct clientArgs{
    int clientSocket;
}ClientThreadArgs;

int exitWithMessage(char* message);
void *get_in_addr(struct sockaddr *sa);
void manageThreadedClient(void* args);
void decreaseClients();