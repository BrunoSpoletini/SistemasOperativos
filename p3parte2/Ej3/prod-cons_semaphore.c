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

sem_t lugares,ocupados;
pthread_mutex_t lk;

void enviar(int *p){

	sem_wait(&lugares);
	
	pthread_mutex_lock(&lk);
	buffer[buffindex] = p;
	buffindex++;
	pthread_mutex_unlock(&lk);

	sem_post(&ocupados);

	return;
}

int * recibir(){

	sem_wait(&ocupados);
		pthread_mutex_lock(&lk);
		int *rta = buffer[buffindex-1];
		buffindex--;
		pthread_mutex_unlock(&lk);
	sem_post(&lugares);

	return rta;
}

void * prodF(void *arg)
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

void * consF(void *arg)
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

	sem_init(&lugares,0,SZ);
	sem_init(&ocupados,0,0);

	pthread_mutex_init(&lk,NULL);

	for (i = 0; i < M; i++)
		pthread_create(&productores[i], NULL, prodF, i + (void*)0);

	for (i = 0; i < N; i++)
		pthread_create(&consumidores[i], NULL, consF, i + (void*)0);

	pthread_join(productores[0], NULL); /* Espera para siempre */

	sem_destroy(&lugares);
	sem_destroy(&ocupados);
	pthread_mutex_destroy(&lk);

	return 0;
}

