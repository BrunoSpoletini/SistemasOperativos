#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//#include <timing.h>
#define SIZE (int)5e7

/*
gcc -fopenmp 2-sumaParalela.c && OMP_NUM_THREADS=2 ./a.out
*/

double lista[SIZE];

int main(){
    int i;
    double total;
    //#pragma parallel for
    //for(int i=0; i<SIZE;i++){lista[i]=1;}

    #pragma omp parallel 
    {
        #pragma omp for reduction(+:total)
        for(i=0; i< SIZE; i++){
            total+=lista[i];
        }
    }
    printf("Suma del arreglo: %f\n", total);
    return 0;
}