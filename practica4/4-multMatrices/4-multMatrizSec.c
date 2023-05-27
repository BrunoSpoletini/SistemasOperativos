#include <stdio.h>
#include <stdlib.h>
#define NUM 1000

int A[NUM][NUM], B[NUM][NUM], C[NUM][NUM];

void mult(int A[NUM][NUM], int B[NUM][NUM], int C[NUM][NUM]){
    int i, j, k;
    for (i = 0; i < NUM; i++)
        for (j = 0; j < NUM; j++)
            for (k = 0; k < NUM; k++)
                C[i][j] += A[i][k] * B[k][j];
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
    srand(7);
    int i, j;
    for (i = 0; i < NUM; i++) {
        for (j = 0; j < NUM; j++) {
            A[i][j] = random() % 1000;
            B[i][j] = random() % 1000;
        }
    }
    mult(A, B, C);
    return 0;
}