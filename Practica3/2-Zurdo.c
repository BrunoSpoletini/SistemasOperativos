#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define N_FILOSOFOS 5
#define ESPERA 5000000
pthread_mutex_t tenedor[N_FILOSOFOS];
pthread_mutex_t * izq(int i) { return &tenedor[i]; }
pthread_mutex_t * der(int i) { return &tenedor[(i+1) % N_FILOSOFOS]; }
void pensar(int i)
{
printf("Filosofo %d pensando...\n", i);
usleep(random() % ESPERA);
}
void comer(int i) {
    printf("Filosofo %d comiendo...\n", i);
    usleep(random() % ESPERA);
}

void tomar_tenedores(int i) {
    if(i==0){
        pthread_mutex_lock(izq(i));
        pthread_mutex_lock(der(i));

    }else{
        pthread_mutex_lock(der(i));
        pthread_mutex_lock(izq(i));
    }

}

void dejar_tenedores(int i) {
    pthread_mutex_unlock(der(i));
    pthread_mutex_unlock(izq(i));
}

void * filosofo(void *arg) {
    int i = arg - (void*)0;
    while (1) {
        tomar_tenedores(i);
        comer(i);
        dejar_tenedores(i);
        pensar(i);
    }
}

int main() {

    pthread_t filo[N_FILOSOFOS];
    int i;
    for (i = 0; i < N_FILOSOFOS; i++)
        pthread_mutex_init(&tenedor[i], NULL);
    for (i = 0; i < N_FILOSOFOS; i++)
        pthread_create(&filo[i], NULL, filosofo, i + (void*)0);
    
    pthread_join(filo[0], NULL);
    return 0;
}

/*
    int sem_init(sem_t *sem, int pshared, unsigned int value);
    int sem_destroy(sem_t *sem);
    int sem_wait(sem_t *sem);
    int sem_post(sem_t *sem);
*/