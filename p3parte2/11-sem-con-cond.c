#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct{
    pthread_mutex_t lock;
    pthread_cond_t verde;
    int contador;
}semC;


// Esto iria en el programa del usuario
semC semCond;
////////

void* sem_initC(semC* semCond, int value){
    pthread_mutex_lock(&semCond->lock);
    semCond->contador = value;
    pthread_cond_init(&semCond->verde, NULL);
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

int sem_postC(semC* semCond){
    pthread_mutex_lock(&semCond->lock);
    semCond->contador++;
    pthread_cond_signal(&semCond->verde);
    pthread_mutex_unlock(&semCond->lock);
}

int sem_trywaitC(semC* semCond){
    pthread_mutex_lock(&semCond->lock);
    if(semCond->contador == 0){
        pthread_mutex_unlock(&semCond->lock);
        return -1;
    }
    semCond->contador--;
    pthread_mutex_unlock(&semCond->lock);
    return 0;
}

void* test(){
    sleep(5);
    for(int i=0; i<3; i++){
        sem_postC(&semCond);
    }
}

int main(){
    pthread_t s1, s2;
    sem_initC(&semCond, 3);

    pthread_create(&s1, NULL, test, NULL);

    for(int i =0; i<5; i++){

        if (sem_trywaitC(&semCond)==0){
        printf("Test\n");
        } else{
            printf("fallo trylock\n");
        }
    }
    pthread_join(s1, NULL);
    return 0;
}