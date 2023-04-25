
#include "panaderia.h"


void init(int threads){
    if(threads >= MAX_THREADS){
        printf("Esta implementacion acepta hasta %d threads\n",MAX_THREADS-1);
        return;
    }
    Nthreads = threads;
}

int cant;
void lock(int index){
    if(Nthreads == 0){
        printf("Es necesario inicializar con init()\n");
        return;
    }

    CHOOSE[index] = true;
    asm("mfence"); // con este el aviso a los demas que estoy eligiendo
                    /// (puede que esto no este en el cache del otro)

    int m = 0;
    for(int i = 0; i < Nthreads; i++ )
        m = max(m,TURN[i]);

    TURN[index] = m+1;
    CHOOSE[index] = false;
    asm("mfence"); /// con este nos aseguramos que para cuando
                    /// les llega la bandera de CHOOSE, TURN ya esta cargado tambien.

    for(int j = 0; j < Nthreads; j++){
        while(CHOOSE[j]){ /// esperamos que termine de elegir.
            /* busy waiting */ 
        }
        

        while ((TURN[j] != 0) && ((TURN[j] < TURN[index]) || ((TURN[j] == TURN[index]) && (j < index)))) {
            /* busy waiting */ 
        }
    }
}


void unlock(int index){
    TURN[index] = 0;
}