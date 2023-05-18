#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t tabaco, papel, fosforos, otra_vez;

void agente(){
    while (1){
        sem_wait(&otra_vez);
        int caso = random() % 3;
        if (caso != 0) sem_post(&fosforos);
        if (caso != 1) sem_post(&papel);
        if (caso != 2) sem_post(&tabaco);
    }
}

void fumar(int fumador){
    printf("Fumador %d: Puf! Puf! Puf!\n", fumador);
    sleep(1);
}

void * fumador1(void *arg){
    while (1){
        sem_wait(&tabaco);
        if (sem_trywait(&papel) == -1){
            sem_post(&tabaco);
        } else {
            fumar(1);
            sem_post(&otra_vez);
        }
    }
}

void * fumador2(void *arg){
    while (1){
        sem_wait(&tabaco);
        if (sem_trywait(&fosforos) == -1){
            sem_post(&tabaco);
        } else {
            fumar(2);
            sem_post(&otra_vez);
        }
    }
}

void * fumador3(void *arg){
    while (1){
        sem_wait(&papel);
        //
        if (sem_trywait(&fosforos) == -1){
            sem_post(&papel);
        } else {
            fumar(3);
            sem_post(&otra_vez);
        }
    }
}

int main(){
    pthread_t s1, s2, s3;

    sem_init(&tabaco, 0, 0);
    sem_init(&papel, 0, 0);
    sem_init(&fosforos, 0, 0);
    sem_init(&otra_vez, 0, 1);

    pthread_create(&s1, NULL, fumador1, NULL);
    pthread_create(&s2, NULL, fumador2, NULL);
    pthread_create(&s3, NULL, fumador3, NULL);

    agente();

    return 0;
}

/*
si pudiese modificarse el agente, le pondria un lock a los dos elementos que otorga

a) 
El deadlock puede ocurrir en el caso en el que un fumador toma uno de los ingredientes del agente, e inmediatamente despues
otro fumador toma el segundo ingrediente, de esta forma se agotan los recursos del agente, ninguno de los procesos puede avanzar,
y se produce el deadlock.

b)
El agente provee dos de los tres ingredientes necesarios para fumar, y los otorga en el siguiente orden:
fosforos, papel, tabaco.

Ahora analizamos los fumadores segun el orden en el que consumen recursos:
fumador 1: tabaco, papel
fumador 2: fosforos, tabaco
fumador 3: papel, fosforos


Para que el fumador consiga fumar, es necesario que obtenga los dos ingredientes del agente.
Supongamos que el primer ingrediente que otorga el agente son fosforos, y que alguno de los fumadores lo consume primero.
Como no existe forma de saber cual sera el segundo ingrediente del agente, siempre existira la posibilidad de que no sea el que
necesita el fumador, por lo que el primer ingrediente del agente debera ser la ultima prioridad de los fumadores.
Si ordenamos los ingredientes de esta forma, los fumadores debaran consumir los ingredientes en el orden inverso al que los 
proporciona el agente (tabaco, papel, fosforos). Esto resulta en el siguiente orden:
fumador 1: tabaco, papel
fumador 2: tabaco, fosforos
fumador 3: papel, fosforos
Pero esto significa que dos fumadores solicitan el mismo recurso primero. Por lo que existe la posibilidad de que cualquiera de los dos lo tome.
En el caso en el que el agente otorgue fosforos y tabaco, si el fumador 1 toma el tabaco primero, ocurrira deadlock.
En el caso en el que el agente otorgue papel y tabaco, si el fumador 2 toma el tabaco primero, ocurrira deadlock.
Por lo tanto no existe forma en la que los recursos puedan ordenarse para evitar el problema.

c) La solucion propuesta se basa en que cada fumador tome primero los ultimos ingredientes que otorga el agente. 
Una vez que lo tomo, intenta tomar su segundo ingrediente, el cual el agente ya deberia haber dejado disponible.
En caso de que no lo pueda tomar, significa que el agente no dio los ingredientes necesarios para ese fumador en particular,
por lo que el fumador suelta el primer ingrediente que agarro, permitiendo que otro lo tome.


*/