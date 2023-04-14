#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include<unistd.h>

#define VIS 10

int visitantes = 0;

void * proc(void *arg) {
    int i;
    int id = arg - (void*)0;
    for (i = 0; i < VIS; i++) {
        int c;
        c = visitantes;
        sleep(0.1);
        visitantes = c + 1;
    }
    return NULL;
}

int main(){
    pthread_t t0, t1;
    pthread_create(&t0, NULL, proc, NULL);
    pthread_create(&t1, NULL, proc, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    printf("Visitantes: %d\n", visitantes);
    return 0;
}