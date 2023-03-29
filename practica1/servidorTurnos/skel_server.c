#define MAX_EVENTS 256
#define READ_SIZE 256
#define MAX_CLIENTS 256
#define TIMEOUT 10000
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
#include <fcntl.h>

#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

/*
 * Para probar, usar netcat. Ej:
 *
 *      $ nc localhost 4040
 *      NUEVO
 *      0
 *      NUEVO
 *      1
 *      CHAU
 */

void quit(char *s)
{
	perror(s);
	abort();
}

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;

	/*
	 * Leemos de a un caracter (no muy eficiente...) hasta
	 * completar una línea.
	 */
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

int handle_conn(int csock, int id)
{
	char buf[READ_SIZE];
	int rc;

	/* Atendemos pedidos, uno por linea */
	rc = fd_readline(csock, buf);
	if (rc < 0)
		quit("read... raro");

	/* Linea vacia, se cerró la conexión */
	if (rc == 0){
		write(csock, "Cliente desconectado\n", 21);
		return -1;
	}

	if (!strcmp(buf, "NUEVO"))
	{	
		char reply[20];
		sprintf(reply, "%d\n", id);
		write(csock, reply, strlen(reply));
	}
	else if (!strcmp(buf, "PRINT"))
	{
		write(csock, "Output check\n", 13);
	}
	else if (!strcmp(buf, "CHAU"))
	{
		write(csock, "Cliente desconectado\n", 21);	
		return -1;
	}
	return 0;
}

void wait_for_clients(int lsock)
{
	int csock[MAX_CLIENTS], run = 1, events_count, bytes_read, csock_count=0;
	char buff[READ_SIZE];

	struct epoll_event events[MAX_EVENTS];
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		quit("Fallo al crear epoll fd\n");
	}
	
	// Registro el socket de servidor en epoll
	events[0].events = EPOLLIN;
	events[0].data.fd = lsock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lsock, &events[0])) {
		fprintf(stderr, "Fallo al agregar fd a epoll\n");
		close(epoll_fd);
		exit(1);
	}

	while(run){

		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);

		for(int i=0; i < events_count; i++){

			if (events[i].data.fd == lsock) {
				csock[csock_count] = accept(lsock, NULL, NULL);
				if (csock[csock_count] < 0){
					quit("accept");
				} else {
					
					// Registro el socket del cliente en epoll 
					events[csock_count+1].events = EPOLLIN;
					events[csock_count+1].data.fd = csock[csock_count];
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock[csock_count], &events[csock_count+1])) {
						fprintf(stderr, "Fallo al agregar fd a epoll\n");
						close(epoll_fd);
						exit(1);
					}
					// Hacemos el nuevo socket no bloqueante
					if (fcntl(csock[csock_count], F_SETFL, O_NONBLOCK) < 0) {
						perror("Fallo al hacer el socket no bloqueante");
						exit(1);
					}
					write(csock[csock_count], "Cliente conectado!\n", 19);
					csock_count++;
				}
			} else {
				for(int j = 0; j  < csock_count; j++){
					if (events[i].data.fd == csock[j]) {
						if (handle_conn(csock[j], j) == -1){
							if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, csock[j], &events[j+1])) {
								fprintf(stderr, "Fallo al quitar fd de epoll\n");
								close(epoll_fd);
								exit(1);
							}
							//close(csock[j]); Si lo cierro falla, pero entiendo que deberia cerrarlo
						}
					} 
				}
			}
		}
	}
}

/* Crea un socket de escucha en puerto 4040 TCP */
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
	sa.sin_port = htons(4040);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 4040 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsock, MAX_CLIENTS);
	if (rc < 0)
		quit("listen");

	return lsock;
}





int main()
{
	int lsock;

	lsock = mk_lsock();

	//mk_epoll(lsock); to do
	
	wait_for_clients(lsock);

}