#include <stdio.h>
#include <omp.h>
#include <math.h>

//
//gcc -fopenmp 3-primalidad.c -lm && OMP_NUM_THREADS=2 ./a.out
long n = 1000000000000000003;

int main(){
    int root, primo=1;

    root = floor(sqrt(n));

    for (int i=2; i < root; i++){
        if( n % i == 0){
            primo = 0;
            break;
    }

    }
    primo? printf("Es primo\n") : printf("No es primo\n");
    return 0;
}