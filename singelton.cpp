#include <pthread.h>
#include <mutex>
#include <iostream>

using namespace std;
pthread_mutex_t my_lock;

template <typename T> class singleton{
private:
    static singleton *my_singel;
    T my_template;

    singleton(T my_template) {
        this->my_template = my_template;
    }

public:
    void Destroy() {
        my_template = NULL;
        pthread_mutex_unlock(&my_lock);    
    }

    static singleton *Instance(T stam_T) {
        if (pthread_mutex_trylock(&my_lock)) {
            return my_singel;
        } 
        else {
            my_singel = new singleton(stam_T);
            return my_singel;
        }   
    }
};

template <typename T> singleton<T> *singleton<T>::my_singel = 0;
int main(){
    FILE *file_p; 
    singleton<FILE *> *first = singleton<FILE *>::Instance(file_p);
    singleton<FILE *> *second = singleton<FILE *>::Instance(file_p);
    if (first == second) {
        cout<<"first: "<<first<<" == second: "<<second;
        cout<<"\n we passed the test :)\n";
        return 1;
    }
    else{
        cout<<"first: "<<first<<" 1= second: "<<second;
        cout<<"\n we didnt passed the test :(\n";
        return 0;
    }
}