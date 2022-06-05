#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;
#define PORT 9034

int main(){
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1){
        printf("couldnt initialize the socket\n");
        return -1;
    }
    struct sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT); 

    int client_socket = connect(server_socket, (struct sockaddr *)&addr_server, sizeof(addr_server));
    if (client_socket == -1){
        printf("couldnt connect to the server\n");
        close(server_socket);
        return -1;
    }
    printf("connected to the server\n type whatever you want to send\n");
    int num_of_bytes = 0;
    int is_runnig = 1;
    while (is_runnig == 1) {
        printf("input: ");
        string input;
        getline(cin, input);
        char *buffer = const_cast<char*>(input.c_str());
        int is_sended = send(server_socket, buffer, 1024, 0);
        if (is_sended == -1){
            printf("couldnt send to the server\n");
            return -1;
        }
        printf("sended '%s' to the server\n", buffer);
        num_of_bytes = recv(server_socket, buffer, 1024, 0);
        if (num_of_bytes == -1){
            printf("error reciving from server\n");
            return -1;
        }
        
        if (num_of_bytes == 0){
                is_runnig = 0;
                break;
        }
        printf("recived from server: %s\n",buffer);
    }
    close(server_socket);
    return 0;
}