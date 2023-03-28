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
	int csock;
	
	/*
	To do:
	Cambiar wait_for_clients a iterativo
	Crear un epoll antes del while
	De aca para abajo meter todo dentro de un while
	Si accept da un fd valido:
		Almacenar los fd de las conecciones en un array de fd
		Agregar el fd del accept al control del epoll
	
	De alguna forma usar epoll para que lea los msjes de los clientes, y conteste con lo que corresponda...?
	*/

	/* Esperamos una conexión, no nos interesa de donde viene */
	csock = accept(lsock, NULL, NULL);





	if (csock < 0){
		quit("accept");
	} else {
		pid_t pid = fork();
		switch (pid)
		{
		case -1:
			quit("Fork fail");
			break;
		case 0: // child
			/* Atendemos al cliente */
			handle_conn(csock, id);
		default:
			/* Cerramos la copia del socket del proceso parent */
			close(csock);
			/* Volvemos a esperar conexiones */
			wait_for_clients(lsock, id+1);
			break;
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
	rc = listen(lsock, 10);
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