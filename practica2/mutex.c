#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define VIS 200000

/*
gcc -pthread mutex.c && ./a.out

*/

int visitados =0;
int usado = 0;
void* fun(){
    

        for(int i = 0; i < VIS; i++){

            while(usado){
            }
        
            usado = 1;
            visitados++;
            usado = 0;
        }

    //return;
}

int main(){

    pthread_t pid1,pid2;
    pthread_create(&pid1,NULL,fun,NULL);
    pthread_create(&pid2,NULL,fun,NULL);

    pthread_join(pid1,NULL);
    pthread_join(pid2,NULL);

    printf("tenemos %d visitantes\n",visitados);

    return 0;
}