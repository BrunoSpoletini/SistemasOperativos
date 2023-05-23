#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//#include <timing.h>
#define MAXN (int)5e8
/*
gcc -fopenmp 1-ornamental.c  && ./a.out

 OMP_NUM_THREADS=2 ./prog
*/
double A[MAXN];

int main(){

    A[50] = 1.2;
    A[1000] = 2.8;

    double suma;
    #pragma omp parallel
    {
    #pragma omp for reduction(+:suma)
    for(int i=0; i< MAXN; i++){
        suma += A[i];
    }
    }

    printf("La suma nos da %f\n", suma);

    return 0;
}