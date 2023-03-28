#define MAX_EVENTS 256
#define READ_SIZE 256
#define MAX_CLIENTS 256
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

//int U = 0;

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

void handle_conn(int csock, int id)
{
	char buf[200];
	int rc;

	while (1)
	{
		/* Atendemos pedidos, uno por linea */
		rc = fd_readline(csock, buf);
		if (rc < 0)
			quit("read... raro");

		if (rc == 0)
		{
			/* linea vacia, se cerró la conexión */
			close(csock);
			return;
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
			write(csock, "Nos vemos\n", 10);
			close(csock);
			exit(0);
		}
	}
}

void wait_for_clients(int lsock, int id)
{
	int csock[MAX_CLIENTS], run = 1, events_count, bytes_read, csock_count=0;
	char buff[READ_SIZE];
	struct epoll_event event, events[MAX_EVENTS];
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		fprintf(stderr, "Fallo al crear epoll fd\n");
		exit(1);
	}
	event.events = EPOLLIN | EPOLLOUT;// Can append "|EPOLLOUT" for write events as well
	event.data.fd = 0;

	events[0].events = EPOLLIN | EPOLLOUT;
	events[0].data.fd = lsock;
	// Registro el socket de servidor en epoll
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lsock, &events[0])) {
		fprintf(stderr, "Fallo al agregar fd a epoll\n");
		close(epoll_fd);
		exit(1);
	}

	// // Registro stdin en epoll
	// if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event)) {
	// 	fprintf(stderr, "Fallo al agregar fd a epoll\n");
	// 	close(epoll_fd);
	// 	exit(1);
	// }
	
	while(run){
		printf("Waiting...\n");

		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 100000);
		printf("%d eventos listos.\n", events_count);

		for(int i=0; i < events_count; i++){

			if (events[i].data.fd == lsock) {
				csock[csock_count] = accept(lsock, NULL, NULL);
				if (csock[i] < 0){
					quit("accept");
				} else {
					csock_count++;
					// Registro el socket del cliente en epoll
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock[csock_count-1], &event)) {
						fprintf(stderr, "Fallo al agregar fd a epoll\n");
						close(epoll_fd);
						exit(1);
					}
					// Hacemos el nuevo socket no bloqueante
					if (fcntl(csock[csock_count-1], F_SETFL, O_NONBLOCK) < 0) {
						perror("Fallo al hacer el socket no bloqueante");
						exit(1);
					}
					printf("Sucess...?\n");
					write(csock[0], "Cliente conectado!!!!\n", 19);
				}

			} else if (events[i].data.fd == 0) {
				printf("Leyendo fd: '%d' - ", events[i].data.fd);
				bytes_read = read(events[i].data.fd, buff, READ_SIZE); 
				printf("%d bytes read.\n", bytes_read);
				buff[bytes_read] = '\0';
				printf("Read '%s'\n", buff);
			
				if(!strncmp(buff, "stop\n", 5))
				run = 0;
			} 

		}





	}
	
	
	
	// 	csock = accept(lsock, NULL, NULL);
	// 	if (csock < 0){
	// 		quit("accept");
	// 	} else {
	
	// 		pid_t pid = fork();
	// 		switch (pid)
	// 		{
	// 		case -1:
	// 			quit("Fork fail");
	// 			break;
	// 		case 0: // child
	// 			/* Atendemos al cliente */
	// 			handle_conn(csock, id);
	// 		default:
	// 			/* Cerramos la copia del socket del proceso parent */
	// 			close(csock);
	// 			/* Volvemos a esperar conexiones */
	// 			wait_for_clients(lsock, id+1);
	// 			break;
	// 		}
	// 	}
	// }
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
	int lsock, id=0;

	lsock = mk_lsock();
	
	wait_for_clients(lsock, id);

}