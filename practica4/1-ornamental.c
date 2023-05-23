#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//#include <timing.h>
#define TOTALES 2
/*
gcc -fopenmp 1-ornamental.c  && ./a.out

 OMP_NUM_THREADS=2 ./prog
*/

int visitantes = 1000000;

int main(){
    int i;
    #pragma omp parallel
    {
    #pragma omp for reduction(*:visitantes)
    for(i=0; i< TOTALES; i++){
        printf("tengo %d\n",visitantes);
        visitantes++;
        printf("tengo %d\n",visitantes);
    }
    }
    printf("Num de visitantes: %d\n", visitantes);

    return 0;
}