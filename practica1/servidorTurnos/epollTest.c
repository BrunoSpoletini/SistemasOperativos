#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event


int main() {

    struct epoll_event event;
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		fprintf(stderr, "Fallo al crear epoll fd\n");
		return 1;
	}


	event.events = EPOLLIN;
	event.data.fd = 0;

    // epoll_ctl(epoll_fd, OP_ARGUMENT, FD_TO_CHECK, &event)
    // en lugar de 0 iria lo q nos devuelve el accept
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event)) {
		fprintf(stderr, "Fallo al agregar fd a epoll\n");
		close(epoll_fd);
		return 1;
	}

	if (close(epoll_fd)) {
		fprintf(stderr, "Fallo al cerrar epoll fd\n");
		return 1;
	}


    return 0;
}