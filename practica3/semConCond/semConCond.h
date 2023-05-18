#ifndef semConCond
#define semConCond

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct{
    pthread_mutex_t lock;
    pthread_cond_t verde;
    int contador;
}semC;

void sem_initC(semC* semCond, int value);
void sem_waitC(semC* semCond);
void sem_postC(semC* semCond);
int sem_trywaitC(semC* semCond);
void* sem_destroyC(semC* semCond);

#include "semConCond.c"
#endif