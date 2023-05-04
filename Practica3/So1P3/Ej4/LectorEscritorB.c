#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#define M 5 /// lectores
#define N 5 /// escritores
#define ARRLEN 100000000


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

int lecturas,escrituras;

pthread_mutex_t writelk;
sem_t sem;

bool WannaWrite;

void * escritor(void *arg) {

    int i;
    int num = arg - (void*)0;
    while (1) {
        sleep(random() % 3);

        pthread_mutex_lock(&writelk);
        WannaWrite = true;

        for(int i = 0; i < M; i++){ /// tomamos los turnos de todos los escritores.
            sem_wait(&sem);
        }

        printf("Escritor %d escribiendo %d\n", num,escrituras);

        for (i = 0; i < ARRLEN; i++)
            arr[i] = num;

        for(int i = 0; i < M; i++){ /// tomamos los turnos de todos los escritores.
            sem_post(&sem);
        }
        escrituras++;
        WannaWrite = false;
        pthread_mutex_unlock(&writelk);
    }


    return NULL;
}

void * lector(void *arg){
    int v, i;
    int num = arg - (void*)0;
    while (1) {

        
        sleep(random() % 3);
        sem_wait(&sem);
        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
            break;
        }
        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d %d\n", num, v,lecturas);
        lecturas++;
        sem_post(&sem);
    }
    return NULL;
}

int main(){

    pthread_t lectores[M], escritores[N];
    int i;

    pthread_mutex_init(&writelk,NULL);
    sem_init(&sem,0,M);
    

    for (i = 0; i < M; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
        pthread_join(lectores[0], NULL); /* Espera para siempre */
    return 0;
}