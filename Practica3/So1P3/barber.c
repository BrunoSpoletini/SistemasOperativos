#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#define maxclients 10000
#define N 10


pthread_mutex_t locksillas,lockbarbero; /// lock de la silla.

pthread_cond_t cliente_en_silla, termino_cortar_barbero, nuevo_turno, hay_clientes;
int sillas = N;

/*

hay que atender en orden.



El cliente abre la puerta, y se fija cuantas sillas disponibles hay.
le tienen que mandar una señal de despertar al babero.
Si hay mas de 0, toma una y se queda en espera, sino, se va.
"se queda en espera": Entra a competir por un lock junto al resto de clientes.

*/

/*
Una barber ́ıa tiene una sala de espera con N sillas y
un barbero. Si no hay clientes para atender, el barbero se pone a dormir. Si un cliente llega y todas
las sillas estan ocupadas, se va. Si el barbero esta ocupado pero hay sillas disponibles, se sienta en
una y espera a ser atendido. Si el barbero esta dormido, despierta al barbero. El cliente y el barbero
deben ejecutar concurrentemente las funciones me_cortan() y cortando() y al terminar los dos ejecutar
concurrentemente pagando() y me_pagan().
Escriba un programa que coordine el comportamiento del barbero y los clientes y expl ́ıquelo.
*/
int turnoclient;
bool ocupados, se_sento_cliente, termino_cortar; 
int turno_atendiendo=-1;

pthread_cond_t pago,listo_cambio;
bool listopago, listo_vuelto;

void cortando(){ /// ya tengo el lock
    termino_cortar = false; /// recien empiezo.
    //pthread_mutex_lock(&lock);

    while(!se_sento_cliente)
        pthread_cond_wait( &cliente_en_silla , &lockbarbero );
    
    printf("B: Le corta el pelo al cliente en la silla\n");
    sleep(2);
    termino_cortar = true;
    pthread_cond_signal(&termino_cortar_barbero);

    pthread_mutex_unlock(&lockbarbero);
}

void me_pagan(){
    pthread_mutex_lock(&lockbarbero);
    listo_vuelto = false;
    while(!listopago)
        pthread_cond_wait(&pago, &lockbarbero);

    printf("B: recibe el pago, le da %d en cambio.\n", rand()%200);
    //sleep( rand()%2 );
    listo_vuelto = true;
    pthread_cond_signal(&listo_cambio);

    pthread_mutex_unlock(&lockbarbero);
}

void printestado(){
    printf("la silla esta:");
    if(se_sento_cliente){
        printf("ocupada\n");
    }else{
        printf("vacia\n");
    }
    printf("El pago esta: ");
    if(listopago){
        printf("listo\n");
    }else{
        printf("por hacerse\n");
    }
    printf("el corte esta:");
    if(termino_cortar){
        printf("terminado\n");
    }else{
        printf("En proceso\n");
    }
}

void *barbero(void *p){

    /// si no hay nadie, entonces duermo.

    while(1){

        pthread_mutex_lock(&locksillas);

        while(sillas == N)
            pthread_cond_wait( &hay_clientes , &locksillas );
        /// si llegamos aca, es nuestro turno
        pthread_mutex_unlock(&locksillas);

        pthread_mutex_lock(&lockbarbero);
        turno_atendiendo++;
        pthread_cond_broadcast(&nuevo_turno);

        cortando();
        me_pagan();

        printestado();

        pthread_mutex_unlock(&lockbarbero);
    }

}


void me_cortan(int cliente){ /// llego con el lock tomado.
    printf("C[%d]: se sienta en la silla\n", cliente);
    se_sento_cliente = true;
    pthread_cond_signal(&cliente_en_silla);

    while(!termino_cortar)
            pthread_cond_wait( &termino_cortar_barbero , &lockbarbero );

    printf("C[%d]: me terminaron de cortar\n", cliente);
    se_sento_cliente = false; /// se para el cliente.
}

void pagando(int cliente){ /// llego con el lock tomado
    printf("C[%d]: Le paga al barbero\n", cliente);

    listopago = true;
    pthread_cond_signal(&pago);

    while(!listo_vuelto)
        pthread_cond_wait(&listo_cambio,&lockbarbero);

    printf("C[%d]: recibe el cambio, saluda y se va\n", cliente);
    listopago = false;
}

void *cliente(void *p){

    int client = p - (void*) 0;

    bool hayLugar = false;
    int turno;
    pthread_mutex_lock(&locksillas);
        if(sillas > 0){
            sillas--;
            turno = turnoclient;
            turnoclient++;
            hayLugar = true;
        }
    pthread_mutex_unlock(&locksillas);

    if(!hayLugar){
        printf("C[%d]: Se retira por falta de lugar\n", client);
        return (void*) 0;
    }

    pthread_cond_signal(&hay_clientes);

    /// tenemos el turno turno.
    pthread_mutex_lock(&lockbarbero);
    while(turno_atendiendo != turno)
        pthread_cond_wait(&nuevo_turno, &lockbarbero);

    me_cortan(client);
    pagando(client);

    pthread_mutex_unlock(&lockbarbero);

    pthread_mutex_lock(&locksillas);
        sillas++;
    pthread_mutex_unlock(&locksillas);

}



int main(){
    srand(time(NULL));

    pthread_t pbarber;
    pthread_t pclient[maxclients];


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
