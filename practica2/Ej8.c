#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#define VIS 20000000
#define nhilos 6

int totales;

bool req1, req2;
int turno;

/*
Peterson.
*/
void * molinete1(){
    for(int i = 0; i < VIS*nhilos / 2 ; i++){ 
        req1 = true;
        turno = 2;
        asm("mfence");
        while(turno == 2 && req2);

        totales++;
        req1 = false;
    } 
}

void * molinete2(){
    for(int i = 0; i < VIS*nhilos / 2; i++){ 
        req2 = true;
        turno = 1;
        asm("mfence");
        while(turno == 1 && req1);
        
        totales++;
        req2 = false;
    } 
}

/*
    incl
*/
static inline void inclLock(int *p) {
asm("lock; incl %0" : "+m"(*p) : : "memory");
}

void * molinete_incl1_lock(){
    for(int i = 0; i < VIS ; i++){ 
        inclLock(&totales);
    } 
}

/*
Sin sincronizacion*/
void * molinete(){
    for(int i = 0; i < VIS ; i++){ 
        totales++;
    } 
}

pthread_mutex_t a = PTHREAD_MUTEX_INITIALIZER;

void* molinete_mutex(){
    for(int i = 0; i < VIS ; i++){ 
        pthread_mutex_lock(&a);
        totales++;
        pthread_mutex_unlock(&a);
    } 
}




int main(int argc, char**argp){

    if(argc == 1){
        printf("pasar como parametro el index de test\n");
        return 0;
    }

    int caso = ((char)argp[1][0])-'0';


    pthread_t p[nhilos];

    switch(caso){

        case 1: /// sin sincronizacion.
            printf("Sin sincronizacion entre hilos\n");
            
            for(int i = 0; i < nhilos; i++){
                pthread_create( &p[i] ,NULL, molinete, NULL );
            }
            for(int i = 0; i < nhilos; i++){
                pthread_join( p[i] ,NULL);
            }
            printf("tenemos %d visitantes.\n",totales);
            break;
        case 2: /// peterson.
            printf("Algoritmo de Peterson\n");
            pthread_t p1,p2;

            pthread_create( &p1 ,NULL, molinete1, NULL );
            pthread_create( &p2 ,NULL, molinete2, NULL );

            pthread_join(p1,NULL);
            pthread_join(p2,NULL);

            printf("tenemos %d visitantes.\n",totales);
            break;
        case 3: /// incllock
            printf("usando incl_lock entre hilos\n");

            for(int i = 0; i < nhilos; i++){
                pthread_create( &p[i] ,NULL, molinete_incl1_lock, NULL );
            }
            for(int i = 0; i < nhilos; i++){
                pthread_join( p[i] ,NULL);
            }
            printf("tenemos %d visitantes.\n",totales);
            break;
        case 4: /// mutex
            printf("usando mutex entre hilos\n");

            for(int i = 0; i < nhilos; i++){
                pthread_create( &p[i] ,NULL, molinete_mutex, NULL );
            }
            for(int i = 0; i < nhilos; i++){
                pthread_join( p[i] ,NULL);
            }
            printf("tenemos %d visitantes.\n",totales);

            break;
        case 5: /// sin multithreading.
            printf("Sin multithreading\n");
            for(int i = 0; i < VIS*nhilos; i++){
                totales++;
            }
            printf("tenemos %d visitantes.\n",totales);
            break;
        default:
        printf("indice invalido, debe estar en rango [1,5]\n");
    }


    return 0;
}
/*

Tiempos de ejecucion:


--
Sin multithreading

tenemos 120000000 visitantes.
real	0m0,198s
--

--
Sin sincronizacion entre hilos

tenemos 31292540 visitantes.
real	0m0,407s
--

--
usando incl_lock entre hilos

tenemos 120000000 visitantes.
real	0m1,955s

--

--
usando mutex entre hilos

tenemos 120000000 visitantes.
real	0m4,795s

--

--
Algoritmo de Peterson:

tenemos 120000000 visitantes.
real	0m5,114s
--




*/