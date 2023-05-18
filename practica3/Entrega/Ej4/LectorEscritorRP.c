#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#define N_LECTORES 100 /// lectores
#define N_ESCRITORES 10 /// escritores
#define ARRLEN 10000000

int arr[ARRLEN];

pthread_mutex_t lock;
int lectores;
pthread_cond_t nadie_leyendo;

void tomar_lock_escritor(){
    pthread_mutex_lock(&lock);
    while(lectores != 0)
        pthread_cond_wait(&nadie_leyendo,&lock);
    
}

void soltar_lock_escritor(){
    pthread_mutex_unlock(&lock);
}

void tomar_lock_lector(){
    pthread_mutex_lock(&lock);
    lectores++;
    pthread_mutex_unlock(&lock);
}

void soltar_lock_lector(){
    pthread_mutex_lock(&lock);
    lectores--;
    if(lectores == 0)
        pthread_cond_signal(&nadie_leyendo);
    pthread_mutex_unlock(&lock);
}


void * escritor(void *arg) {

    int i;
    int num = arg - (void*)0;
    while (1) {
        sleep(random() % 3);

        tomar_lock_escritor();

        printf("Escritor %d escribiendo\n", num);
        for (i = 0; i < ARRLEN; i++)
            arr[i] = num;
        
        soltar_lock_escritor();
    }


    return NULL;
}

void * lector(void *arg){
    
    int v, i;
    int num = arg - (void*)0;

    while (1) {
        sleep(random() % 3);

        tomar_lock_lector();

        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
            break;
        }
        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d \n", num, v);

        soltar_lock_lector();
    }
    return NULL;
}

int main(){

    pthread_t lectores[N_LECTORES], escritores[N_ESCRITORES];
    int i;

    pthread_mutex_init(&lock,NULL);
    pthread_cond_init(&nadie_leyendo, NULL);

    for (i = 0; i < N_LECTORES; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N_ESCRITORES; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
    
    pthread_join(lectores[0], NULL); /* Espera para siempre */
    
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&nadie_leyendo);

    return 0;
}
/*
En esta solución, lo que tenemos es una variable "lectores" que nos dice cuantos de ellos hay actualmente en el archivo.
Siempre que un nuevo lector quiera entrar a leer, va a competir por un lock y aumentar en 1 esta variable.

Los escritores, por su parte, deben esperar a que no hayan lectores (lectores = 0).
De no haber ninguno, procede a escribir, de lo contrario, queda dormido a la espera de la condicion "nadie_leyendo".
Es por eso que, cuando un lector termina de leer, revisa si es el unico que estaba leyendo el archivo, y de ser asi, despierta a algun escritor que
esperaba la condicion "nadie_leyendo".
*/