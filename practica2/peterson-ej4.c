#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#define VIS 200000

int totales;

bool req1, req2;
int turno;
void * molinete1(){
    for(int i = 0; i < VIS ; i++){ 
        req1 = true;
        turno = 2;
        asm("mfence");
        while(turno == 2 && req2);

        totales++;
        printf("Mol 1\n");
        req1 = false;
    } 
}

void * molinete2(){
    for(int i = 0; i < VIS; i++){ 
        req2 = true;
        turno = 1;
        asm("mfence");
        while(turno == 1 && req1);
        
        totales++;
        printf("Mol 2\n");
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