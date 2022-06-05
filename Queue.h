#pragma once
#include <stdbool.h>
#include <pthread.h>

#define nullptr ((void*)0)

typedef struct node {
    void* data;
    struct node *next;
}node;


typedef struct Queue {
    node *head;
    node *tail;
    size_t size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    void* deq_res;
}Queue;


Queue* createQ();
void destroyQ(Queue*);
void enQ(void*, Queue*);
void* deQ(Queue*);


