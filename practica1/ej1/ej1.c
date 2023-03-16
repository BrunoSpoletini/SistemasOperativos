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
	// (Para ver esto, se puede crear otro programa que muestre su pid, y llamarlo con un exec desde d())
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
		printf("PID1: %d\n", getpid());
		exit(0);
		break;
	default: //Parent
		printf("PID2: %d\n", getpid());
		sleep(1000);
		break;
	}
	/*
	Con el comando ps -au vemos que el hijo queda en status Z+. 
	Es responsabilidad del padre leer el codigo de finalizacion del hijo,
	por lo que si el hijo termina antes, se quedara en estado zombie, esperando
	que el padre termine. 
	Si el padre termina antes que el hijo, el hijo se volvera huerfano,
	y el proceso init sera el nuevo parent.
	*/
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
	/*
	Luego de hacer el malloc:
	PID USUARIO   PR  NI    VIRT    RES    SHR S  %CPU  %MEM     HORA+ ORDEN                    
  30027 bruno     20   0  979336    944    852 S   0,0   0,0   0:00.00 a.out   

	Luego de un rato de escribir en la memoria previamente solicitada:
	PID USUARIO   PR  NI    VIRT    RES    SHR S  %CPU  %MEM     HORA+ ORDEN  
  30027 bruno     20   0  979336   2836   1480 S  21,3   0,1   0:01.82 a.out                   

	
	*/

}

//g) ¿Qué pasa con el uso de memoria de un proceso al realizar fork()? ¿Y exec()?
void g(){
  	if (1) { // 1 para fork(), 2 para exec()
		char* buff = malloc(1e+9);	
		for (int i=0; i<1e+9; i++){
			buff[i]='A';
		}

		pid_t pid = fork();

		switch (pid)
		{
		case -1:
			fprintf(stderr, "Fork error\n");
			break;
		case 0: //Child
			printf("Pid Child: %d\n", getpid());
			sleep(1000);
			break;
		default: //Parent
			printf("Pid Parent: %d\n", getpid());
			wait(NULL);
			break;
		}
		/*


		Pid Child: 32669
		Pid Parent: 32668
		PID USUARIO   PR  NI    VIRT    RES    SHR S  %CPU  %MEM     HORA+ ORDEN                    
  	  32669 bruno     20   0  979336 977060    512 S   0,0  25,6   0:04.11 a.out                    
  	  32668 bruno     20   0  979336 977464    888 S   0,0  25,6   0:04.15 a.out  

		Se copia la memoria del proceso padre al proceso hijo.
		
		*/

	} else {
		char* buff = malloc(1e+9);	
		for (int i=0; i<1e+9; i++){
			buff[i]='A';
		}

		printf("Pid Proceso original: %d\n", getpid());
		getchar(); // Para revisar el estado del proceso antes de ejecutar exec

		execl("./aux.out", "aux.out", NULL);
		
		/*

		PID USUARIO   PR  NI    VIRT    RES    SHR S  %CPU  %MEM     HORA+ ORDEN                    
	  32145 bruno     20   0  979336 977680   1072 S   0,0  25,6   0:04.54 a.out 

		Memoria del proceso auxiliar:
		PID USUARIO   PR  NI    VIRT    RES    SHR S  %CPU  %MEM     HORA+ ORDEN                    
	  32145 bruno     20   0    2772    968    876 S   0,0   0,0   0:04.60 aux.out

		Vemos como la memoria no se copia al hacer un exec

		*/
	}

}



//h) ¿Qué pasa con los file descriptor de un proceso al hacer fork()? ¿Y exec()?

void h(){
  	if (1) { // 1 para fork(), 2 para exec()
		write(1, "STDOUT antes del fork\n", 22);

		pid_t pid = fork();

		switch (pid){
		case -1:
			fprintf(stderr, "Fork error\n");
			break;
		case 0: //Child
			write(1, "STDOUT despues del fork, desde el child\n", 40);
			sleep(1);
			close(1);
			break;
		default: //Parent
			write(1, "STDOUT despues del fork, desde el parent\n", 41);
			sleep(2);
			write(1, "STDOUT despues del fork, desde el parent, luego de cerrar el fd(1) en el child\n", 79);
			wait(NULL);
			break;
		}
	} else {
		write(1, "STDOUT antes del exec\n", 22);
		close(1); // Cerramos la salida estandar
		execl("./aux.out", "aux.out", NULL);
	}
	/*
	En este caso cerramos el fd(1) desde el child, y desde el parent, luego de 2 segundos, escribimos exitosamente
	por el fd(1). Con esto vemos que los fd se copian al realizar el fork, por lo que podemos trabajar con
	ellos de forma independiente.

	En cambio, al usar exec, cerramos el fd(1), y con exec ejecutamos otro programa, que intenta escribir en fd(1) y falla.
	Esto ocurre porque todos los files descriptors se preservan a traves de las llamadas a exec*.
	*/
}


//i) El comando de cambio de directorio cd suele ser un built-in de la shell. ¿Puede implementarse mediante un programa al igual que, ej., ls?
void i(){

	printf("Uso de programa cd\n");
	execl("./cd.out","cd.out", "/usr/" , NULL);
//TO DO
}



int main() {
	/* Pendientes:
		1) i)
	*/
	i();
	


	
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

