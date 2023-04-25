#include "panaderia.h"
#include <pthread.h>
#include <stdio.h>
#define VIS 2000000
#define threads 4

int totales;

void * molinete(void* arg){
    int index = *((int*)arg);
    printf("tenemos el index %d\n",index);

    for(int i = 0; i < VIS ; i++){
        lock(index);
        totales++;
        //printf("sumo");
        unlock(index);
    } 
}

int main(){

    pthread_t pid[threads];

    int A[threads];

    init(threads);

    for(int i = 0; i < threads; i++){
        A[i] = i;
        pthread_create(&pid[i],NULL,molinete,&A[i]);
    }

    for(int i = 0; i < threads; i++){
        pthread_join(pid[i],NULL);
    }

    printf("hay %d visitantes totales (deberian haber %d) \n",totales,VIS*threads);
    return 0;
}
