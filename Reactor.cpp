#include "Reactor.hpp"
#include <stdlib.h>

Reactor::Reactor() {}

Reactor::~Reactor(){
    delete this;
}

void Reactor::RemoveHandler() {
    this->FileD = 0;
    this->method = NULL;
    pthread_cancel(this->thread);
    this-> ~Reactor();
}

void Reactor::installHandler(void *(method)(void *), int* FileD) {
    this->method = method;
    this->FileD = *FileD;
    pthread_create(&this->thread, NULL, method, (void*)FileD);    
}

