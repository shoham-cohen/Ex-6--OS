#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

class Reactor {
private:
    Reactor();
    ~Reactor();

    pthread_t thread;
    int FileD;
    void*(*method)(void*);

public:
    friend void* newReactor();
    void RemoveHandler();
    void installHandler(void *(method)(void *), int* FileD);

};