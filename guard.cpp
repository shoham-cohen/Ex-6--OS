#include <pthread.h>
#include <unistd.h>
#include <iostream>


using namespace std;
int my_static_var = 0;
pthread_mutex_t my_lock;

class my_guard{
public:

    my_guard(){
        pthread_mutex_lock(&my_lock);
    }

    ~my_guard(){
        pthread_mutex_unlock(&my_lock);
    }
};

void *increas_static_var(void* Thread)
{
    my_guard guard;
    cout<< "Thread number: " << *((int*)Thread) << " is starting the method\n";
    for (int i = 0; i < 50; i++){
        my_static_var++;
    }
    
    cout<< "Thread number: " << *((int*)Thread) << " is finished\n";
    cout<<"The current value is: "<<my_static_var<<" \n";
    return NULL;
}
int main(int argc, char const *argv[]){
    cout<<"we will build 3 threads that is going to be called to the same method and ++ the var 50 times.\n \n";
    sleep(2);
    int init = pthread_mutex_init(&my_lock, NULL);
    if (init != 0){
        printf("failed to initialize mutex\n");
        return 0;
    }
    
    pthread_t my_first_thread; 
    pthread_t my_second_thread;
    pthread_t my_third_thread;
    int one = 1;
    int two = 2;
    int three = 3;
    pthread_create(&my_first_thread, NULL, &increas_static_var, &one);
    pthread_create(&my_second_thread, NULL, &increas_static_var, &two);
    pthread_create(&my_third_thread, NULL, &increas_static_var, &three);


    pthread_join(my_first_thread, NULL);
    pthread_join(my_second_thread, NULL);
    pthread_join(my_third_thread, NULL);

    pthread_mutex_destroy(&my_lock);
    return 0;
}
