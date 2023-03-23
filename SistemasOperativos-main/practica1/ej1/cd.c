#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>


/* Ejercicio 1) h)

Archivo de prueba

Modo de ejecucion por consola:
gcc cd.c -o cd.out && ./cd.out DIR_NAME



*/
int main(int argc, char *argv[]){

    if (argc != 2) {
        fprintf(stderr, "El programa necesita un parámetro.\n");
        return EXIT_FAILURE;
    }

    char dirName[1000] = "--working-directory=";
    strcat(dirName, argv[1]);    

    printf("%s\n",dirName);
    execl("/usr/bin/gnome-terminal", "gnome-terminal", dirName, NULL);

    return 0;
}