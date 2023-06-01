#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define SIZE 10 //(int)5e7

// gcc -fopenmp 2-minimoOMP.c && OMP_NUM_THREADS=2 ./a.out

int lista[]={1000,3,7,234,357,234,55451,6,457, 345};


int min(int a, int b){
    if(a < b)
        return a;
    return b;
}


int main(){

    int minimo = lista[0];
    #pragma omp parallel
    {
        #pragma omp for reduction(min:minimo)
        for(int i=0; i < SIZE; i++){
            printf("%d, ", lista[i]);
            minimo = minimo < lista[i] ? minimo : lista[i];
        }
    }

    printf("El valor minimo es: %d\n", minimo);

    return 0;
}