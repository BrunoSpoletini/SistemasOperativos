#include <stdio.h>
#include <pthread.h>
#include <assert.h>
/*
gcc -pthread locks.c && ./a.out
for i in {0..100000}; do ./a.out; done
Problemas de threads
cahce procesador
orden de instrucciones de assembler

*/

int x = 0, y = 0, a = 0, b = 0;
void * foo(void *arg) { 
    x = 1;
    //asm("mfence");
    a = y;
    return NULL;
}
void * bar(void *arg) { 
    y = 1; 
    //asm("mfence");
    b = x; 
    return NULL; 
}
int main(){
    pthread_t t0, t1;
    pthread_create(&t0, NULL, foo, NULL);
    pthread_create(&t1, NULL, bar, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    assert (a || b);
    return 0;
}


