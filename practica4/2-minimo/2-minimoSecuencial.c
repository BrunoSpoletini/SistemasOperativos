#include <stdio.h>
#include <stdlib.h>

#define SIZE (int)5e7

int lista[SIZE];

int main(){

    int minimo = lista[0];

    for(int i=0; i < SIZE; i++){
        minimo = minimo < lista[i] ? minimo : lista[i];
    }

    printf("El valor minimo es: %d\n", minimo);

    return 0;
}