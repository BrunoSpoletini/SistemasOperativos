#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define M 5
#define N 5
#define SZ 8

/*
 * El buffer guarda punteros a enteros, los
 * productores los consiguen con malloc() y los
 * consumidores los liberan con free()
 */
int *buffer[SZ];
int buffindex;

bool isFull(){
	return buffindex == SZ;
}

bool isEmpty(){
	return buffindex == 0;
}

pthread_mutex_t lk;
pthread_cond_t non_full_cond, non_empty_cond;
void enviar(int *p){

	pthread_mutex_lock(&lk);
	while( isFull() )
		pthread_cond_wait(&non_full_cond, &lk);
	
	buffer[buffindex] = p;
	buffindex++;
	pthread_cond_signal(&non_empty_cond);
	pthread_mutex_unlock(&lk);

	

	return;
}

int * recibir()
{
	/* ???? */

	int *rta;
	pthread_mutex_lock(&lk);
	while( isEmpty() )
		pthread_cond_wait(&non_empty_cond, &lk);
	
	rta = buffer[buffindex-1];
	buffindex--;
	pthread_cond_signal(&non_full_cond);
	pthread_mutex_unlock(&lk);

	return rta;
}

void * prod_f(void *arg)
{
	int id = arg - (void*)0;
	while (1) {
		sleep(random() % 3);

		int *p = malloc(sizeof *p);
		*p = random() % 100;
		printf("Productor %d: produje %p->%d\n", id, p, *p);
		enviar(p);
	}
	return NULL;
}

void * cons_f(void *arg)
{
	int id = arg - (void*)0;
	while (1) {
		sleep(random() % 3);

		int *p = recibir();
		printf("Consumidor %d: obtuve %p->%d\n", id, p, *p);
		free(p);
	}
	return NULL;
}

int main()
{
	pthread_t productores[M], consumidores[N];
	int i;

	pthread_cond_init(&non_full_cond,NULL);
	pthread_cond_init(&non_empty_cond,NULL);

	for (i = 0; i < M; i++)
		pthread_create(&productores[i], NULL, prod_f, i + (void*)0);

	for (i = 0; i < N; i++)
		pthread_create(&consumidores[i], NULL, cons_f, i + (void*)0);

	pthread_join(productores[0], NULL); /* Espera para siempre */
	return 0;
}

