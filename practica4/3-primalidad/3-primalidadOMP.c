#include <stdio.h>
#include <omp.h>
#include <math.h>

//
//gcc -fopenmp 3-primalidadOMP.c -lm && OMP_NUM_THREADS=2 ./a.out
/*
Números primos de 64 bits:

9223372036854775783
9223372036854775837
9223372036854775843
Números semiprimos de 64 bits:

844424930131968
1183741261411601
4294967296723937
Los números semiprimos son productos de dos números primos diferentes.

Números pares de 64 bits:

9223372036854775806
9223372036854775810
9223372036854775814
*/
long n = 1000000000000000003;

int main(){
    int root, primo=1;

    root = ceil(sqrt(n));
    #pragma omp parallel
    {
        //int soy = omp_get_thread_num();
        #pragma omp for
        for (int i=2; i <= root; i++){
            //printf("%d %d\n",soy, i);
            if( n % i == 0){
                primo = 0;
                // El break no funciona con OpenMP
            }
        }

    }
    primo? printf("Es primo\n") : printf("No es primo\n");
    return 0;
}