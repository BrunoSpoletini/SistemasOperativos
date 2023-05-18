#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#define M 50 /// lectores
#define N 50 /// escritores
#define ARRLEN 10000000


/*

-No puede haber un lector accediendo al arreglo al mismo tiempo que un escritor.
-Varios lectores pueden acceder al arreglo simult ́aneamente.
-Solo puede haber un escritor a la vez
*/
int arr[ARRLEN];


/*
Read - Preferring: mientras que haya gente queriando leer, que lean.
Solamente cuadno no haya gente leyendo, se pone el writer a escribir.
Para eso, marcamos una bandera/semaforo de "wantWrite", esperamos a que no hayan
lectores, y tomamos un lock


Write-Preferring: Cuando un writer quiere escribir, levanta la bandera de escritura,
entonces, los lectores terminan de leer pero no vuelven a entrar.
una vez que terminaron todos de leer, el/los writers escriben.
Una vez que terminaron, bajan la bandera de escritura.

Justa: Metemos las consultas en un buffer, en orden en que aparecen.
Y vamos sacando a medida que les corresponde.


*/


pthread_mutex_t lock;
int lectores;
pthread_cond_t nadie_leyendo;

void tomar_lock_escritor_a(){
    pthread_mutex_lock(&lock);
    while(lectores != 0)
        pthread_cond_wait(&nadie_leyendo,&lock);
    
}

void soltar_lock_escritor_a(){
    pthread_mutex_unlock(&lock);
}

void tomar_lock_lector_a(){
    pthread_mutex_lock(&lock);
    lectores++;
    pthread_mutex_unlock(&lock);
}

void soltar_lock_lector_a(){
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
        tomar_lock_escritor_a();

        printf("Escritor %d escribiendo\n", num);

        for (i = 0; i < ARRLEN; i++)
            arr[i] = num;
        soltar_lock_escritor_a();
    }


    return NULL;
}

void * lector(void *arg){
    int v, i;
    int num = arg - (void*)0;
    while (1) {

        
        sleep(random() % 3);

        tomar_lock_lector_a();

        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
            break;
        }
        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d \n", num, v);

        soltar_lock_lector_a();
    }
    return NULL;
}

int main(){

    pthread_t lectores[M], escritores[N];
    int i;

    pthread_mutex_init(&lock,NULL);

    for (i = 0; i < M; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
        pthread_join(lectores[0], NULL); /* Espera para siempre */
    return 0;
}