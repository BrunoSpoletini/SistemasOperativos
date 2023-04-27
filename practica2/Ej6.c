#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include<unistd.h>

#define VIS 10000000

int visitantes = 0;

void * proc() {
    for (int i = 0; i < VIS; i++) 
        visitantes++;
    return NULL;
}

int main(){
    pthread_t t0, t1;
    pthread_create(&t0, NULL, proc, NULL);
    pthread_create(&t1, NULL, proc, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    printf("Visitantes: %d\n", visitantes);
    return 0;
}

/*

Si compilamos con -O, el for adentro de proc se reduce a visitantes = 10000000;
Como eso se ejecuta 2 veces,pero es solo una asignacion, la cuenta final da erronea.

Si compilamos con -O2, el for adentro de proc se reduce a visitantes += 10000000;
Sigue levantando los 2 threads, pero no se puede producir condicion de carrera, ya que se resume a un addl, que
es atomico.

*/