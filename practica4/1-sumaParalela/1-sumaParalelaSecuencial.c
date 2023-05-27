#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define SIZE (int)5e7

double lista[SIZE];

int main(){
    int i;
    double total;

    for(i=0; i< SIZE; i++){
        total+=lista[i];
    }

    printf("Suma del arreglo: %f\n", total);
    return 0;
}