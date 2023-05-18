#include "semConCond.h"

void sem_initC(semC* semCond, int value){
    pthread_mutex_init(&semCond->lock, NULL);
    pthread_cond_init(&semCond->verde, NULL);
    pthread_mutex_lock(&semCond->lock);
    semCond->contador = value;
    pthread_cond_init(&semCond->verde, NULL);
    pthread_mutex_unlock(&semCond->lock);
}

void sem_waitC(semC* semCond){
    pthread_mutex_lock(&semCond->lock);
    while(semCond->contador == 0){
        pthread_cond_wait(&semCond->verde, &semCond->lock);
    }
    semCond->contador--;
    pthread_mutex_unlock(&semCond->lock);
}

void sem_postC(semC* semCond){
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

void* sem_destroyC(semC* semCond){
    pthread_mutex_destroy(&semCond->lock);
    pthread_cond_destroy(&semCond->verde);
}
