#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/*
gcc -pthread mutex.c && ./a.out
El proceso que quiere tomar el mutex se BLOQUEA hasta que lo consigue
trylock retorna si no lo puede tomar, y te permite seguir trabajando
spinlock es para cuando el lock se va a desbloquear ràpido

*/



#define VIS 200000




int visitados = 0;
int usado = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* mol1(){
    for(int i = 0; i < VIS; i++){
        pthread_mutex_lock(&mutex);
        visitados++;
        printf("T1: %d\n", visitados);
        pthread_mutex_unlock(&mutex);
    }
}

void* mol2(){
    for(int i = 0; i < VIS; i++){
        pthread_mutex_lock(&mutex); 
        visitados++;
        printf("T2: %d\n", visitados);
        pthread_mutex_unlock(&mutex);
    }
}

int main(){

    pthread_t m1, m2;
    pthread_create(&m1,NULL,mol1,NULL);
    pthread_create(&m2,NULL,mol2,NULL);

    pthread_join(m1,NULL);
    pthread_join(m2,NULL);

    printf("tenemos %d visitantes\n",visitados);

    return 0;
}