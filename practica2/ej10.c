#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t a = PTHREAD_MUTEX_INITIALIZER;

/*
El hilo creado y el principal acceden a la zona critica al mismo tiempo.

Puntos destacables del man 5 mutex:
- Los mutex deben ser globales (para que sean visibles desde todos los threads)

Por lo que lei, los mutex no se copian.
Intentar copiar un mutex es como intentar copiar una parte del hardware, o un handler del so de muy bajo nivel.


*/

void * foo(void *_arg){
    pthread_mutex_lock(&a);
    printf("Foo!\n");
    sleep(1);
    pthread_mutex_unlock(&a);
    return NULL;
}

int main(){
    pthread_t th;
    pthread_create(&th, NULL, foo, NULL);
    pthread_mutex_t b = a;
    pthread_mutex_lock(&b);
    printf("Bar!\n");
    sleep(1);
    pthread_mutex_unlock(&b);
    pthread_join(th, NULL);
    return 0;
}