#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <linux/types.h>

#define VIS 200000
#define nhilos 100
int totales;

bool req1, req2;
int turno;
void * molinete(){
    for(int i = 0; i < VIS ; i++){
        int prev = totales; /// el pasado de parametros no es atomico.
        bool bien = __atomic_compare_exchange_n (&totales , &prev, prev+1, false, __ATOMIC_RELEASE, __ATOMIC_ACQUIRE);
        if(!bien)
            i--;
    }
}


int main(){

    pthread_t p [nhilos];

    for(int i = 0; i < nhilos; i++){
        pthread_create( &p[i] ,NULL, molinete, NULL );
    }


    for(int i = 0; i < nhilos; i++){
        pthread_join( p[i] ,NULL);
    }


    printf("tenemos %d visitantes.\n",totales);


    return 0;
}
