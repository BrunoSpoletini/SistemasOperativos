#include <stdio.h>
#include <pthread.h>


typedef struct{
    pthread_mutex_t lock;
    pthread_cond_t verde;
    int contador;
}semC;

void* sem_initC(semC* semCond, int value){
    pthread_mutex_lock(&semCond->lock);
    semCond->contador = value;
    pthread_cond_init(&semCond->verde, NULL);
    if ( value > 0 ){
        pthread_cond_signal(&semCond->verde);
    }
    pthread_mutex_unlock(&semCond->lock);
}

void* sem_waitC(semC* semCond){
    pthread_mutex_lock(&semCond->lock);
    while(semCond->contador == 0){
        pthread_cond_wait(&semCond->verde, &semCond->lock);
    }
    semCond->contador--;
    pthread_mutex_unlock(&semCond->lock);
}

void* sem_postC(){

}

void* sem_trywaitC(){

}


int main(){
    pthread_t s1, s2;
    semC semCond;
    sem_initC(&semCond, 3);

    
    for(int i =0; i<5; i++){
        sem_waitC(&semCond);
        printf("Test\n");
    }
    
    


    //pthread_create(&s1, NULL, fun, NULL);

    return 0;
}