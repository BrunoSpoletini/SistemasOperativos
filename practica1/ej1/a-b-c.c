#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>


// Ej. 1: Conteste las siguientes preguntas. Puede ser util usar strace para monitorear las llamadas a
// sistema de un proceso, o top para ver la tabla de procesos del sistema.

// a) Si se cierra el file descriptor de la salida estándar (1), ¿qué pasa al escribir en el mismo?
void a(){

	int ret1, ret2;

	ret1 = write(1, "Prueba output 1\n",16);

	close(1); 

	ret2 = write(1, "Prueba output 2\n",16);

	fprintf(stderr, "Prueba 1: %d\nPrueba 2: %d\n", ret1, ret2);
}
// Al cerrarse el file descriptor de la salida estandar, el comando de escritura retorna un error (-1)


//b) Si se cierra el file descriptor de la entrada estándar (0), ¿qué pasa al intentar leer del mismo?
void b(){
    char buff[100];
	for (int i=0; i<100; i++){buff[i]='a';} //Seteamos el array para no tener caracteres basura

	close(0); //Cerramos la entrada estandar (stdin)

    fgets( buff, 100, stdin );

	buff[strlen(buff)] = '\0';

	printf("%s\n", buff);

} // Al cerrar la entrada estandar, la funcion fgets retorna NULL, al no poder leer caracteres de stidn


//c) Si un file descriptor se duplica con dup(), ¿qué pasa al cerrar una de las copias?
void c(){
	int ret1, ret2;

	int stdout2 = dup(1); /// duplica el fd (Se tienen que cerrar los dos.)

	write(1, "Prueba salida estandar 1\n",25);

	close(1); 

	write(stdout2, "Prueba salida estandar 2\n",25);

	fprintf(stderr, "Prueba 1: %d\nPrueba 2: %d\n", ret1, ret2);

	// Al cerrar una de las copias, aun puede escribirse en la otra. Deben cerrarse ambos fd.
}


//d) Al hacer fork(), ¿cómo cambia el valor de getpid()? ¿Y al hacer exec()?
void d(){
	int pid = fork(); //obviamos el caso de error
	if (pid == 0) { //Child
		printf("Pid of child: %d\n", getpid());
	} else {
		printf("Pid of parent: %d\n", getpid());
		wait(NULL);
	}
	// Al hacer exec se mantiene el pid actual.
}


//e) Con fork(), cree dos procesos y haga que el hijo termine (con exit() o retornando del main)
// y que el padre duerma indefinidamente sin hacer wait(). ¿Cómo aparece el hijo en la tabla de procesos? 
// ¿Por qué sigue existiendo?

void e(){
	pid_t pid = fork();
	switch (pid)
	{
	case -1:
		fprintf(stderr, "Fork error\n");
		break;
	case 0: //Child
		printf("PID1: %d", getpid());
		exit(0);
		break;
	default: //Parent
		printf("PID2: %d", getpid());
		sleep(1000);
		break;
	}
	// TO DO --- No logro encontrar el proceso child en la tabla
}


//f) Al hacer un malloc de 1GB, ¿aumenta el uso de memoria de un proceso? Explique.
void f(){

	printf("PID: %d\n", getpid()); //El \n es necesario, porque sino el buffer del stdout no se llena antes de llamar a sleep
	char* buff = malloc(1e+9);
	sleep(5); // Tiempo para ejecutar el comando "top -p PID"
	for (int i=0; i<1e+9; i++){
		printf("Out\n");
		buff[i]='A';
	}
	// El proceso reserva la memoria virtual, pero la memoria fisica comienza a asignarse realmente
	// mientras se va utilizando.
	

}
/*
g) ¿Qué pasa con el uso de memoria de un proceso al realizar fork()? ¿Y exec()?
h) ¿Qué pasa con los file descriptor de un proceso al hacer fork()? ¿Y exec()?
i) El comando de cambio de directorio cd suele ser un built-in de la shell. ¿Puede implementarse mediante un programa al igual que, ej., ls?
*/




int main() {
	
	f();
	


	
	/*
	int rc;

    int stdin2 = dup(1); /// duplica el fd (Se tienen que cerrar los dos.)

	//close(1); 
	rc = write(1, "asdasd\n", 7); /// si hay error (fd cerrado) entonces  rc = 1.
	fprintf(stderr, "rc=%i\n", rc);
	exit(0);
	*/

	return 0;
}

