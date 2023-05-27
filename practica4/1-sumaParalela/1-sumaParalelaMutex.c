#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE (int)5e7


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

double lista[SIZE], total;
int cont;

void* suma(){
    pthread_mutex_lock(&lock);
    total += lista[cont];
    cont++;
    pthread_mutex_unlock(&lock);
}

int main(){
    pthread_t hilo1, hilo2;
    
    pthread_create(&hilo1, NULL, suma, NULL);
    pthread_create(&hilo2, NULL, suma, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    pthread_mutex_destroy(&lock);

    printf("Suma del arreglo: %f\n", total);

}