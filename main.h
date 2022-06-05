#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>


#define PORT "1700"  
#define BACKLOG 30  


struct inp_obj {
    char input[1024];
    int fd;
};

void* enQfirst(void* );
void* AO1_func(void* );
void* AO1_finish(void* );
void* AO2_func(void* );
void* AO2_finish(void* );
void* AO3_func(void* );