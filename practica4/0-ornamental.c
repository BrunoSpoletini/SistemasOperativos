#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//#include <timing.h>
#define TOTALES 10000000

/*
gcc -fopenmp 1-ornamental.c && OMP_NUM_THREADS=2 ./a.out
*/

int visitantes = 0;

int main(){
    int i;
    #pragma omp parallel 
    {
        #pragma omp for reduction(+:visitantes)
        for(i=0; i< TOTALES; i++){
            visitantes++;
        }
    }
    printf("Num de visitantes: %d\n", visitantes);
    return 0;
}