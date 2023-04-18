#define MAX_EVENTS 256
#define READ_SIZE 256
#define MAX_CLIENTS 256
#define TIMEOUT 100
#define MAX_THREADS 4
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>	   // Para hacer un socket no bloqueante
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <sys/wait.h>  //Para usar wait
#include <string.h>	   // strtok
#include <pthread.h>
#include "hash_table.h"
/*
Para inicializar la hash table:
	HashTable table;
	initHashTable(&table);

	insert(&table, "apple", 1);

 * Para probar, usar netcat. Ej:
 *      $ nc localhost 3942

Posibles mejoras:
Hacer un sistema de salida, tanto para el cliente como para el serivdor.
Para esto se puede usar pthread_exit(0);

Checkeo de errores? Revisar

Comentado de codigo

Borrar comentarios de ayuda

Mejorar el parseo del input

Unificar todo a camelCase

 */

int lsock;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t arrMutex[MAX_THREADS];

int cSocks[MAX_THREADS];

HashTable hTable;

void quit(char *s)
{
	perror(s);
	abort();
}

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;
	while ((rc = read(fd, buf + i, 1)) > 0)
	{
		if (buf[i] == '\n')
			break;
		i++;
	}

	if (rc < 0)
		return rc;

	buf[i] = 0;
	return i;
}

int handle_conn(int csock)
{
	HashTable *table = &hTable;
	char buf[READ_SIZE], *token, **args, clave[100];
	int rc, size;

	/* Atendemos pedidos, uno por linea */
	rc = fd_readline(csock, buf);
	if (rc < 0)
		quit("Fallo al leer");

	// Parseo del input
	args = malloc(sizeof(char *) * 3);
	if (args == NULL)
		quit("Fallo malloc");
	token = strtok(buf, " \n"); /// separa segun espacio y \n
	for (int i = 0; token != NULL; i++)
	{
		args[i] = malloc(sizeof(token) + 1);
		if (args[i] == NULL)
			quit("Fallo malloc");
		strcpy(args[i], token);
		token = strtok(NULL, " \n");
		args[i + 1] = NULL;
	}

	/* Linea vacia, se cerró la conexión */
	if (rc == 0)
	{
		write(csock, "Cliente desconectado\n", 21);
		return -1;
	}

	if (!strcmp(args[0], "PUT"))
	{ // PUT test 123
		pthread_mutex_lock(&mutex);
		insert(table, args[1], atoi(args[2]));
		pthread_mutex_unlock(&mutex);
		write(csock, "OK\n", 4);
	}
	else if (!strcmp(args[0], "DEL"))
	{
		pthread_mutex_lock(&mutex);
		delete (table, args[1]);
		pthread_mutex_unlock(&mutex);
		write(csock, "Clave-valor eliminado exitosamente\n", 35);
	}
	else if (!strcmp(args[0], "GET"))
	{
		if (get(table, args[1]) == -1)
		{
			write(csock, "No existe clave para el valor ingresado\n", 40);
		}
		else
		{
			pthread_mutex_lock(&mutex);
			sprintf(clave, "%d", get(table, args[1]));
			size = sizeof(get(table, args[1]));
			pthread_mutex_unlock(&mutex);
			write(csock, clave, size);
			write(csock, "\n", 1);
		}
	}
	return 0;
}

void agregarClienteEpoll(int csock, int epoll_fd){
	// Registro el socket del cliente en epoll
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = csock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
	write(csock, "Cliente conectado!\n", 19);
}


void wait_for_clients(int lsock, int epoll_fd, int threadId)
{
	int run = 1, events_count, bytes_read;
	char buff[READ_SIZE];
	struct epoll_event events[MAX_EVENTS], ev;

	while (run)
	{
		// Checkeamos si hay clientes nuevos y los agregamos a la lista de obs de epoll
		if (pthread_mutex_trylock(&arrMutex[threadId]) == 0){
			if(cSocks[threadId] != 0){
				agregarClienteEpoll(cSocks[threadId], epoll_fd);
				cSocks[threadId] = 0;
			}
			pthread_mutex_unlock(&arrMutex[threadId]);
		}
		
		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		for (int i = 0; i < events_count; i++){
			int csock = events[i].data.fd;
			if (handle_conn(csock) == -1)
			{
				if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, csock, NULL))
				{
					close(epoll_fd);
					quit("Fallo al quitar fd de epoll\n");
				}
				close(csock);
			}
		}
	}
}

/* Crea una instancia de epoll y agrega lsock a la lista de control */
int create_epoll(int lsock)
{
	struct epoll_event ev;
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		quit("Fallo al crear epoll fd\n");
	}

	// Registro el socket de servidor en epoll
	ev.events = EPOLLIN;
	ev.data.fd = lsock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lsock, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
	return epoll_fd;
}

/* Crea un socket de escucha en puerto 3942 TCP */
int mk_lsock()
{
	struct sockaddr_in sa;
	int lsock;
	int rc;
	int yes = 1;

	/* Crear socket */
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(3942);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 3942 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsock, MAX_CLIENTS);
	if (rc < 0)
		quit("listen");

	return lsock;
}

void *handleThread(void *args)
{
	int threadId = *(int*)args;
	printf("El id es: %d", threadId);

	struct epoll_event ev;
	int epoll = epoll_create1(0);
	if (epoll == -1)
	{
		quit("Fallo al crear epoll fd\n");
	}

	wait_for_clients(lsock, epoll, threadId);
}

int main(){
	int epoll, clientC = 0, ids[MAX_THREADS];
	pthread_t t[MAX_THREADS];
	initHashTable(&hTable);

	lsock = mk_lsock();

	for (int i = 0; i < MAX_THREADS; i++){
		// Guardamos en memoria el id que recibira el thread
		ids[i] = i;

		// Array donde se le enviaran los sockets de los clientes a los hilos
		cSocks[i] = 0; 

		// Inicializamos los mutexes que bloquearan dicho array
		pthread_mutex_init(&arrMutex[i], NULL);

		// Creamos los threads y les pasamos un id unico como parametro
		pthread_create(&(t[i]), NULL, handleThread, (void*)&ids[i]);
	}

	while(1){ // Aceptamos clientes y los "mandamos" a los hilos correspondientes
		int csock = accept(lsock, NULL, NULL);
		if (csock == -1){
			quit("Fallo al aceptar un cliente");
		} else {
			clientC++;
			pthread_mutex_lock(&arrMutex[clientC%MAX_THREADS]);
			cSocks[clientC%MAX_THREADS] = csock;
			pthread_mutex_unlock(&arrMutex[clientC%MAX_THREADS]);
		}
	}

	return 0;
}