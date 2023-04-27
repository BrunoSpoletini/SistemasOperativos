#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#define VIS 2000000

int totales;

bool req1, req2;
int turno;

/*

En caso de que uno solo de los procesos quiera entrar a la zona critica, entra (ya que la flag del otro esta
en false).

En caso de que ambos quieran entrar al mismo tiempo:
Cambiando el mfence al lugar entre el flag y el turno,
Ambos procesos son conscientes de que el otro quiere entrar a la zona critica, pero
no son conscientes del turno global.

Como puede que no haya sincronizacion entre variables de los hilos,
El molinete 1 piensa que es turno del 2, y el 2 piensa que es turno del 1.
De esta manera, se quedan los 2 esperando.
Hasta que se realiza una sincronizacion de variables entre los hilos. Y entonces alguno de los dos accede
a la zona critica.

Es decir, ninguno de los dos procesos va a avanzar hasta que la operacion "turno = x" del otro proceso no le
llegue a su memoria de hilo.

Por lo tanto, poner el mfence entre ambas lineas es posible, solo que ambos quedarian en un "spinlock"
hasta que la variable turno sea actualizada en ambos procesos.

*/

void * molinete1(){
    for(int i = 0; i < VIS ; i++){ 
        req1 = true;
        asm("mfence");
        turno = 2;
        while(turno == 2 && req2);

        totales++;
        //printf("Mol 1\n");
        req1 = false;
    } 
}

void * molinete2(){
    for(int i = 0; i < VIS; i++){ 
        req2 = true;
        asm("mfence");
        turno = 1;
        while(turno == 1 && req1);
        
        totales++;
        //printf("Mol 2\n");
        req2 = false;
    } 
}


int main(){

    pthread_t p1,p2;

    pthread_create( &p1 ,NULL, molinete1, NULL );
    pthread_create( &p2 ,NULL, molinete2, NULL );

    pthread_join(p1,NULL);
    pthread_join(p2,NULL);

    printf("tenemos %d visitantes.\n",totales);


    return 0;
}