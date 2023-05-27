#include <stdio.h>
#include <stdlib.h>
#define NUM 1000
/*
gcc -fopenmp 4-multMatrizOMP.c && time OMP_NUM_THREADS=2 ./a.out && gcc 4-multMatrizSec.c && time ./a.out 
El secuencial funciona mas rapido y no entiendo bien pq
*/

int A[NUM][NUM], B[NUM][NUM], C[NUM][NUM];

void mult(int A[NUM][NUM], int B[NUM][NUM], int C[NUM][NUM]){
    int i, j, k;
    #pragma omp parallel
    {
        #pragma omp for
        for (i = 0; i < NUM; i++)
            //#pragma omp for
            for (j = 0; j < NUM; j++)
                //#pragma omp for reduction(+,C[i][j])
                for (k = 0; k < NUM; k++)
                    C[i][j] += A[i][k] * B[k][j];
    }
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
    mult(A, B, C);
    return 0;
}