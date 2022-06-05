#include "Queue.h"
#include "active_object.h"
#include "main.h"


Queue* createQ(){
    
    Queue* Q = (Queue*)malloc(sizeof(Queue));
    Q->head = nullptr;
    Q->tail = nullptr;
    Q->deq_res = nullptr;
    Q->size = 0;
    pthread_mutex_init(&Q->lock, NULL);
    pthread_cond_init(&Q->cond, NULL);
    return Q;
}

void destroyQ(Queue* Q) {
    
    while (Q->size != 0){ 
        deQ(Q);
    }
    if (Q->deq_res != NULL){
        free(Q->deq_res);
    }
    pthread_cond_broadcast(&Q->cond);
    pthread_cond_destroy(&Q->cond);
    pthread_mutex_destroy(&Q->lock);
    free(Q);
}

void enQ(void* n, Queue* Q) {

    pthread_mutex_lock(&(Q->lock));
    node* newNode = (node*)malloc(sizeof(node));
    newNode->data = malloc(1032);
    newNode->next = nullptr;
    memcpy(newNode->data, n, 1032);
    if(Q->size != 0){
        Q->size++;
        Q->tail->next = newNode;
        Q->tail = newNode;
        pthread_mutex_unlock(&(Q->lock));
    }
    else{
        Q->head = newNode;
        Q->tail = newNode;
        Q->size++;
        pthread_mutex_unlock(&(Q->lock));
        pthread_cond_signal(&Q->cond);
    }
}

void* deQ(Queue* Q) {
   
    pthread_mutex_lock(&(Q->lock));
    if (Q->size==0) {
        pthread_cond_wait(&Q->cond, &Q->lock);
    }
    
    if (Q->deq_res!=NULL) {
        free(Q->deq_res);
    }

    Q->deq_res = malloc(1032);
    memcpy(Q->deq_res, Q->head->data, 1032);
    if (Q->size == 1) {
        free(Q->head->data);
        free(Q->head);
    }

    else{
        node* node_to_deq = Q->head;
        Q->head = Q->head->next;
        free(node_to_deq->data);
        free(node_to_deq);
    }

    Q->size--;
    pthread_mutex_unlock(&(Q->lock));
    return Q->deq_res;
}

active_object* newAO(Queue* Q, void* first_func, void* second_func) {
    num++;
    active_object *AO = (active_object*)malloc(sizeof(active_object));
    AO->Q = Q;
    AO->is_alive = 1;
    AO->first_func = first_func;
    AO->second_func = second_func;
    AO->handle_thread = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(AO->handle_thread,NULL, handle, (void*)AO);
    printf("AO number %d has initialized succesfully\n", num);
    return AO;
}

void destroyAO(active_object* AO) {
    AO->is_alive = 0;
    free(AO->handle_thread);
    free(AO);
}


void* handle(void* newAO) {
    active_object *AO = (active_object*)newAO;
    while (AO->is_alive == 1) { 
        void* tmp = AO->first_func(deQ(AO->Q)); 
        AO->second_func(tmp);
    }
    free(AO->handle_thread);
    free(AO);
}

pthread_t threads_list[BACKLOG];
int clients_list[BACKLOG];
int sockfd; 
bool is_connected;
Queue* Q_1;
active_object *AO_1;
Queue* Q_2;
active_object *AO_2; 
Queue* Q_3;
active_object *AO_3;

void* enQfirst(void* i) {
    enQ(i, Q_1);
    return i;
}

void* AO1_func(void* i) {
    struct inp_obj* current_res = (struct inp_obj* )i;
    int len = strlen(current_res->input);
    for (size_t j = 0; j < len; j++)
    {
        if (current_res->input[j] >= 'a' && current_res->input[j] <= 'z') {
            current_res->input[j] = (current_res->input[j]-96)%26+97;
        } 
        else if (current_res->input[j]>= 'A' && current_res->input[j] <= 'Z') {
            current_res->input[j] = (current_res->input[j]-64)%26+65;
        }
    }
    return current_res;
}

void* AO1_finish(void* i) {
    enQ(i, Q_2);
    return i;
}

void* AO2_func(void* i) {
    struct inp_obj *current_res = (struct inp_obj*)i;
    int len = strlen(current_res->input);
    for (size_t j = 0; j < len; j++)
    {
        if (current_res->input[j]>='a'&&current_res->input[j]<='z') {
            current_res->input[j] -= 32;
        } 
        else if (current_res->input[j] >= 'A' && current_res->input[j] <= 'Z') {
            current_res->input[j] += 32;
        }
    }
    return i;
}

void* AO2_finish(void* i) {
    enQ(i, Q_3);
    return i;
}

void* AO3_func(void* i) {
    return i;
}

void* AO3_finish(void* i) {
    struct inp_obj* res = (struct inp_obj*)i;
    int clients_list = res->fd;
    if (send(clients_list, res->input, 1024, 0) == -1)  {
        perror("send");
    }
    printf("sending: %s to client number %d\n", res->input, clients_list);
    return NULL;
}


bool check_if_valid(char* input) {
    for (int i = 0; i < strlen(input); i++){
        if (isprint(input[i]) == 0) {
            return false;
        }
    }
    return true;    
}

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *thread_handle(void *client){

    int clients_list = *(int*)client;
    pthread_detach(pthread_self());
    int bytes_recived;
    char buffer[1024];
    bool is_connected2 = true;
    struct inp_obj* inp;
    while (is_connected2 && is_connected) {
        bytes_recived = recv(clients_list, buffer, sizeof(buffer), 0);
        if (bytes_recived <= 0) {
            perror("error recieving the message from the client\n");
            is_connected2 = false;
            pthread_exit(NULL);
            break;
        }
        *(buffer+bytes_recived) = '\0';
        if (!check_if_valid(buffer)) {
            if (send(clients_list, "input is not valid", 1024, 0) == -1)  {
                perror("error sending the message to the client\n");

            }
        } 
        else {
            inp = (struct inp_obj*)malloc(sizeof(struct inp_obj));
            inp->fd = clients_list;
            memcpy(inp->input, buffer, sizeof(buffer));
            printf("client number %d sent: %s\n", inp->fd, inp->input);
            enQfirst(inp);
        }
    }
    free(inp);
    close(clients_list);
}

void sig_handler(int signum)
{
    if (signum == SIGINT) {
        printf("program terminating\n");
        is_connected = false;
        for (size_t i = 0; i < BACKLOG; i++)
        {
            close(clients_list[i]);
        }

        close(sockfd);
        for (size_t i = 0; i < BACKLOG; i++)
        {
            pthread_cancel(threads_list[i]);
        }
        printf("socket closed\n");
        destroyAO(AO_1);
        destroyAO(AO_2);
        destroyAO(AO_3);
        destroyQ(Q_1);
        destroyQ(Q_2);
        destroyQ(Q_3);


        printf("the program terminated succesfully");
        exit(0);
    }

}


int main(void)
{
    Q_1 = createQ();
    AO_1 = newAO(Q_1, AO1_func,AO1_finish);
    Q_2 = createQ();
    AO_2 = newAO(Q_2, AO2_func , AO2_finish);
    Q_3 = createQ();
    AO_3 = newAO(Q_3, AO3_func, AO3_finish);
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;

    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    int j = 0;
    signal (SIGINT,sig_handler);
    is_connected = true;

        while(1) {
        sin_size = sizeof their_addr;
        clients_list[j] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (clients_list[j] == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_create(&threads_list[j%BACKLOG], NULL, thread_handle, &clients_list[j]);
        j++;
    }

    return 0;
}