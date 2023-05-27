#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define SIZE (int)5e7

// gcc -fopenmp 2-minimo.c && OMP_NUM_THREADS=2 ./a.out

int lista[SIZE];


int main(){

    int minimo = lista[0];

    #pragma omp parallel lastprivate(minimo)
    {
        #pragma omp for lastvalue
        for(int i=0; i < SIZE; i++){
            minimo = minimo < lista[i] ? minimo : lista[i];
        }
    }

    printf("El valor minimo es: %d\n", minimo);

    return 0;
}