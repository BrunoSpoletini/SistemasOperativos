#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


/// hay 2 procesos, y dos recursos compartidos.
/// cada uno toma un recurso y los dos necesitan los 2.

pthread_mutex_t l1,l2;

void *f(){
    pthread_mutex_lock(&l1);
    for(int i =0; i< 10000; i++){
        i++;
        i--;
    }
    pthread_mutex_lock(&l2);
    //hago algo
    pthread_mutex_unlock(&l1);
    pthread_mutex_unlock(&l2);
}


void *g(){
    pthread_mutex_lock(&l2);
    pthread_mutex_lock(&l1);
    //hago algo
    pthread_mutex_unlock(&l1);
    pthread_mutex_unlock(&l2);
}
int main(){

pthread_t t1,t2;

pthread_create(&t1,NULL,f,NULL);
pthread_create(&t2,NULL,g,NULL);

pthread_join(t1,NULL);
pthread_join(t2,NULL);

printf("volvi, no hay deadlock\n");

    return 0;
}