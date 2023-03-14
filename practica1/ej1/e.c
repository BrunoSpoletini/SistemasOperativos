#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main()
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
		abort();

	if (pid == 0) {
		printf("Hijo: soy %d\n", getpid());
		exit(1);
	} else {
		printf("Padre: soy %d\n", getpid());
		pause();
	}
}

/// con el comando ps -au vemos que el hijo queda en status Z+.
