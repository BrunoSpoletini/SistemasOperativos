#define MAX_EVENTS 256
#define READ_SIZE 256
#define MAX_CLIENTS 256
#define TIMEOUT 100
#define MAX_THREADS 4
#include <stdio.h>
#include <stdlib.h>
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
#include <unistd.h>
/*
 * Para probar, usar netcat. Ej:
 *      $ nc localhost 8000
 *      NUEVO
 *      0
 *      NUEVO
 *      1
 *      CHAU
 */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int ticketId = 0;
int lsock;

void quit(char *s)
{
	perror(s);
	abort();
}

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;
	while ((rc = read(fd, buf + i, 1)) > 0) {
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
	char buf[READ_SIZE];
	int rc;

	/* Atendemos pedidos, uno por linea */
	rc = fd_readline(csock, buf);
	if (rc < 0)
		quit("read... raro");

	/* linea vacia, se cerró la conexión */
	if (rc == 0)
	{
		return -1;
	}
	if (!strcmp(buf, "NUEVO"))
	{	
		char reply[20];
		pthread_mutex_lock(&mutex);
		sprintf(reply, "%d\n", ticketId);
		ticketId++;
		pthread_mutex_unlock(&mutex);
		write(csock, reply, strlen(reply));
	}
	else if (!strcmp(buf, "CHAU"))
	{	
		return -1;
	}

}

void re_agregarClienteEpoll(int csock, int epoll_fd){
	// Registro el socket del cliente en epoll
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLONESHOT;
	ev.data.fd = csock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, csock, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
}


void *wait_for_clients(void *epoll)
{
	int events_count, epoll_fd = *(int*)epoll;
	struct epoll_event events[MAX_EVENTS];

	while (1)
	{
		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		for (int i = 0; i < events_count; i++){
			if (events[i].data.fd != lsock){
				int csock = events[i].data.fd;
				if (handle_conn(csock) == -1)
				{
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, csock, NULL) == -1)
					{
						close(epoll_fd);
						quit("Fallo al quitar fd de epoll\n");
					}
					close(csock);
				}else{
					re_agregarClienteEpoll(csock,epoll_fd);
				}
			}
		}
	}
}

void agregarClienteEpoll(int csock, int epoll_fd){
	// Registro el socket del cliente en epoll
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLONESHOT;
	ev.data.fd = csock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
}

/* Crea una instancia de epoll y agrega lsock a la lista de control */
int create_epoll()
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

/* Crea un socket de escucha en puerto 8000 TCP */
int mk_lsock()
{
	struct sockaddr_in sa;
	int lsockT;
	int rc;
	int yes = 1;

	/* Crear socket */
	lsockT = socket(AF_INET, SOCK_STREAM, 0);
	if (lsockT < 0)
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsockT, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(8000);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 8000 TCP, en todas las direcciones disponibles */
	rc = bind(lsockT, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsockT, MAX_CLIENTS);
	if (rc < 0)
		quit("listen");

	return lsockT;
}

int main()
{

	pthread_t t[MAX_THREADS];

	lsock = mk_lsock();

	int epoll = create_epoll();

	for (int i = 0; i < MAX_THREADS; i++){
		pthread_create(&(t[i]), NULL, wait_for_clients, (void*)&epoll);
	}

	while(1){ // Aceptamos clientes y los agregamos al epoll
		int csock = accept(lsock, NULL, NULL);
		if (csock == -1){
			quit("Fallo al aceptar un cliente");
		} else {
			agregarClienteEpoll(csock, epoll);
		}
	}

    return 0;
}