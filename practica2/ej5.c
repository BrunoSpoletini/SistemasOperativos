#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#define VIS 20000000

int totales;

/*
Correr un programa con un solo procesador
taskset -c 0 ./a.out

*/

bool req1, req2;
int turno;
void * molinete1(){
    for(int i = 0; i < VIS ; i++){ 
        req1 = true;
        turno = 2;
        //asm("mfence");
        while(turno == 2 && req2);

        totales++;
        //printf("Mol 1\n");
        req1 = false;
    } 
}

static inline void incl(int *p) {
asm("incl %0" : "+m"(*p) : : "memory");
}

void * molinete_single_core1(){
    for(int i = 0; i < VIS ; i++){ 
        totales++;
    } 
}

void * molinete_single_core2(){
    for(int i = 0; i < VIS ; i++){ 
        totales++;
    } 
}

void * molinete_single_core_incl1(){
    for(int i = 0; i < VIS ; i++){ 
        incl(&totales);
    } 
}

void * molinete_single_core_incl2(){
    for(int i = 0; i < VIS ; i++){ 
        incl(&totales);
    } 
}


void * molinete2(){
    for(int i = 0; i < VIS; i++){ 
        req2 = true;
        turno = 1;
        ///asm("mfence");
        while(turno == 1 && req1);
        
        totales++;
        //printf("Mol 2\n");
        req2 = false;
    } 
}


int main(){

    pthread_t p1,p2;
    pthread_create( &p1 ,NULL, molinete_single_core_incl1, NULL );
    pthread_create( &p2 ,NULL, molinete_single_core_incl2, NULL );

    pthread_join(p1,NULL);
    pthread_join(p2,NULL);

    //printf("tenemos %d visitantes.\n",totales);

    assert(totales == 2*VIS);



    return 0;
}

/*
a) En un solo core, no tenemos paralelismo, pero si concurrencia. Si existe un cambio de hilo
luego de la operacion de lectura, pero antes de la de escritura, es posible que obtengamos menos
visitantes de los esperados.

b) Al tener un solo core, los dos hilos corren sobre el mismo core, de esta manera, no es necesario
sincronizar la cache del core con ningun otro. Por ello, no es necesario el mfence.

c) 



*/