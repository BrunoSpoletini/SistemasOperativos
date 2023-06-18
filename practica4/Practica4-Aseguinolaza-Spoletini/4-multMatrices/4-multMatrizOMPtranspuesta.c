#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NUM 2000
/*
gcc -fopenmp 4-multMatrizOMPtranspuesta.c -o multOPM && time OMP_NUM_THREADS=2 ./multOPM && gcc 4-multMatrizSec.c -o multSEC && time ./multSEC 
El secuencial funciona mas rapido y no entiendo bien pq
*/

int A[NUM][NUM], B[NUM][NUM], C[NUM][NUM], D[NUM][NUM];

void mult(int A[NUM][NUM], int B[NUM][NUM], int C[NUM][NUM]){
    int i;
    #pragma omp parallel for
    for (i = 0; i < NUM; i++)
        for (int j = 0; j < NUM; j++) /// la j y la k deben estar declarados dentros del for
            for (int k = 0; k < NUM; k++) /// para que asi cada procesador la tome como privada.
                C[i][j] += A[i][k] * B[j][k];
}

void print(int C[NUM][NUM]){
    for(int i=0; i<NUM;i++){
        for(int j=0;j<NUM;j++){
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
}

int main(){
    int i, j;
    srand(7);
    for (i = 0; i < NUM; i++) {
        for (j = 0; j < NUM; j++) {
            A[i][j] = random() % 1000;
            B[i][j] = random() % 1000;
        }
    }

    // Transponemos la matriz B y almacenamos la transpuesta en D
    for(int i = 0; i < NUM ; i++){
		for(int j = 0; j < NUM; j++){
			D[i][j] = B[j][i];
		}
	}

    mult(A, D, C); // Multiplicamos A x B_T

    return 0;
}