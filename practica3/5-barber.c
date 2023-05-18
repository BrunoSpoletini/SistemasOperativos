#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#define maxclients 10000
#define Nsillas 10

struct pepe{
    int a;
    int b;
};

/*
Para atenderlos en orden, cada uno toma un turno.
*/

/*
Una barberıa tiene una sala de espera con N sillas y
un barbero. Si no hay clientes para atender, el barbero se pone a dormir. Si un cliente llega y todas
las sillas estan ocupadas, se va. Si el barbero esta ocupado pero hay sillas disponibles, se sienta en
una y espera a ser atendido. Si el barbero esta dormido, despierta al barbero. El cliente y el barbero
deben ejecutar concurrentemente las funciones me_cortan() y cortando() y al terminar los dos ejecutar
concurrentemente pagando() y me_pagan().
Escriba un programa que coordine el comportamiento del barbero y los clientes y expliquelo.
*/

sem_t pago, cambio,sentado,sillas,termino_cortar, cola;
pthread_cond_t nuevoturno;

pthread_mutex_t ruletaturnos,turno;
int turnode;
int ultimo_turno;

void cortando(){ /// ya tengo el lock

    sem_wait(&sentado);
    
    printf("B: Le corta el pelo al cliente en la silla\n");
    sleep(1);

    sem_post(&termino_cortar);
}

void me_pagan(){

    sem_wait(&pago);
    printf("B: recibe el pago, le da %d en cambio.\n", rand()%200);
    sem_post(&cambio);

}

void *barbero(void *p){

    while(1){

        sem_wait(&cola); /// esperamos a que haya alguien en la cola, mientras tanto dormimos


        /// aviso a todos que hay un turno disponible, que
        turnode++;/// es el turno del siguiente
        asm("mfence");
        pthread_cond_broadcast(&nuevoturno);

        cortando();
        me_pagan();

    }

}


void me_cortan(int cliente){ /// llego con el lock tomado.
    printf("C[%d]: se sienta en la silla\n", cliente);
    sem_post(&sentado);

    sem_wait(&termino_cortar);

    printf("C[%d]: me terminaron de cortar\n", cliente);
}

void pagando(int cliente){ /// llego con el lock tomado
    printf("C[%d]: Le paga al barbero\n", cliente);

    sem_post(&pago);

    sem_wait(&cambio);

    printf("C[%d]: recibe el cambio, saluda y se va\n", cliente);
}


void *cliente(void *p){

    int client = p - (void*) 0;

    if( sem_trywait(&sillas) == 0 ){ /// lo lockeamos

        /// sacamos un turno
        int miturno;

        pthread_mutex_lock(&ruletaturnos);
            ultimo_turno++;
            miturno = ultimo_turno;
        pthread_mutex_unlock(&ruletaturnos);

        sem_post(&cola); /// nos ponemos en la cola

        pthread_mutex_lock(&turno);

        while(turnode != miturno)
            pthread_cond_wait(&nuevoturno, &turno);

        sem_post(&sillas); /// liberamos una silla.

        me_cortan(client);
        pagando(client);

        pthread_mutex_unlock(&turno);


    }else{
        printf("C[%d]: Se retira por falta de lugar\n", client);
        return (void*) 0;
    }

}



int main(){
    srand(time(NULL));

    pthread_t pbarber;
    pthread_t pclient[maxclients];

    /// barreras:
    sem_init(&sentado,0,0); /// indica si se sento un cliente
    sem_init(&sillas,0,Nsillas); // indica cuantas sillas hay disponibles
    sem_init(&cola,0,0); /// indica cuantas sillas estan ocupadas
    sem_init(&pago,0,0); /// indica si el cliente ya preparo el pago
    sem_init(&cambio,0,0); /// indica si el barbero ya preparo el cambio.

    pthread_create(&pbarber, NULL, barbero, NULL);

    int indexc = 0;
    while(1){
        sleep( rand()%2 );
        pthread_create(&pclient[indexc], NULL, cliente, indexc + (void*) 0 );
        indexc++;
    }

    pthread_join(pbarber,NULL);


    return 0;
}
