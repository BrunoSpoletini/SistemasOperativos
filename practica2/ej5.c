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
static inline void incl(int *p) {
    asm("incl %0" : "+m"(*p) : : "memory");
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

static inline void inclLock(int *p) {
asm("lock; incl %0" : "+m"(*p) : : "memory");
}

void * molinete_single_core_incl1_lock(){
    for(int i = 0; i < VIS ; i++){ 
        inclLock(&totales);
    } 
}

void * molinete_single_core_incl2_lock(){
    for(int i = 0; i < VIS ; i++){ 
        inclLock(&totales);
    } 
}

int main(){

    pthread_t p1,p2;
    while(true){
        totales = 0;

        pthread_create( &p1 ,NULL, molinete_single_core_incl1_lock, NULL );
        pthread_create( &p2 ,NULL, molinete_single_core_incl2_lock, NULL );

        pthread_join(p1,NULL);
        pthread_join(p2,NULL);

        //printf("tenemos %d visitantes.\n",totales);

        assert(totales == 2*VIS);
    }
    return 0;
}

/*
a) En un solo core, no tenemos paralelismo, pero si concurrencia. Si existe un cambio de hilo
luego de la operacion de lectura, pero antes de la de escritura, es posible que obtengamos menos
visitantes de los esperados.

b) Al tener un solo core, los dos hilos corren sobre el mismo core, de esta manera, no es necesario
sincronizar la cache del core con ningun otro. Por ello, no es necesario el mfence.

c) Al tener un solo core y utilizar la instruccion incl de x86, no se generan problemas. 
Esto ocurre porque, el incremento de la variable visitantes utilizando la instruccion de assembler, es atomico.
Por lo que no puede generarse el error en el que se ejecuten dos instrucciones de lectura sucesivas antes de incrementar el contador.

d) Al tener mas de un procesador, y usando la instruccion incl, puede generarse un error. Esto sucede porque existe la posibilidad de que un hilo
incremente la variable, pero esta quede almacenada en la cache del mismo, mientras que el otro hilo incrementa una variable desactualizada.

e) Con la nueva version de la funcion incl, no se genera error. 
Esto sucede porque esta nueva version incluye el prefijo de instruccion lock, que hace [alguna cosa rara a nivel hadrware],
y evita el problema del cache del procesador. 
--The LOCK prefix ensures that the CPU has exclusive ownership of the appropriate cache line for the duration of the operation, 
and provides certain additional ordering guarantees.





*/