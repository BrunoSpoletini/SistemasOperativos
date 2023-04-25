#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
/*
gcc -fopenmp ej12ParalelFor.c && ./a.out
*/


/*
El orden de ejecucion puede variar, pero el resultado final es el mismo.
Por definicion, no hay condicion de carrera.
*/

bool R[256];
bool R2[256];

void charsof(char *s, int len, bool *r){
    int i;
    for(i = 0; i < 256; i++)
        r[i] = false;
    /// parallel for.

    #pragma omp parallel for
    for(i = 0; i < len;i++)
        r[ s[i] ] = true;
}

int main(){
    
    /*
    #pragma omp parallel for
    for(int i = 0; i < 1000000; i++){
        int tid = omp_get_thread_num();
        printf("Id thread :%d, iteracion: %d\n",tid, i);
    }
    */

   
   char *s = "LOISDAHF3453Q39128JNSCKVJZX,L3453124531324533425";
   charsof(s, strlen(s), R);


    for(int i = 0; i < strlen(s); i++){
        R2[ s[i] ] = true;
    }

    for(int i = 0; i < 256; i++ )
        assert( R[i] == R2[i] );

    printf("TODO OK\n");

    return 0;
}