#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include<unistd.h>

#define VIS 100000

int visitantes = 0;

/*
El thread 1 entra primero, hacemos que este espere a que el segundo lea la variable.
Despues, ejecutamos N-1 operaciones y esperamos a que el otro haga todas las suyas

0 - 
1 - lee 1, lee 2
2 - corre 1 N-1 veces,
3 - corre 2 una vez, pone rta en 1.
4 - lee 1 el 1.
5 - corre todo 2
6 - 1 escribe 2.

*/
void * proc(void *arg) {
    int id = (arg - (void*)0);
    for (int i = 0; i < VIS; i++) {
        printf("soy el thread %d y estoy en la iteracion %d\n",id,i);
        int c;
        if( i == (VIS-1) && id == 1 ){ /// si estoy en la ultima iteracion de 1, va a ser la que ponga el 2
            printf("1 termino sus N-1 iteraciones, espera que 2 ponga un 1\n",id);
            sleep(2);
        }

        
        if(i == 1 && id == 2){ 
            printf("2 le da tiempo a 1 de leer el 1\n",id);
            sleep(2);
        }
        


        c = visitantes;
        if( i == 0 ){
            printf("%d espera a que el otro lea\n",id);
            sleep(1); /// esperamos a que ambos lean.
        }

        if( i == (VIS-1) && id == 1 ){ /// si estoy en la ultima iteracion de 1, va a ser la que ponga el 2
            printf("1 espera a que el 2 haga todas sus iteraciones\n",id);
            sleep(2);
            printf("Y ahora 1 escribe el 2");
        }

        if(i == 0 && id == 2){ // el 2 espera las iteraciones del otro
             printf("2 espera que 1 haga N-1 iteraciones\n",id);
            sleep(1);
        }
        visitantes = c + 1;
    }
    return NULL;
}

int main(){
    pthread_t t0, t1;
    pthread_create(&t0, NULL, proc, 1 );
    pthread_create(&t1, NULL, proc, 2);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    printf("Visitantes: %d\n", visitantes);
    return 0;
}