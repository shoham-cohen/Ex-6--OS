#include "Queue.h" 
#include <pthread.h>

static int num = 0;
typedef struct active_object {

    int is_alive; //0 = dead 1 = alive
    pthread_t *handle_thread;
    Queue *Q;
    void* (*first_func)(void*);
    void* (*second_func)(void*);

}active_object;

active_object *newAO(Queue* Q, void* first_func, void* second_func);
void* handle(void*);
void destroyAO(active_object*);