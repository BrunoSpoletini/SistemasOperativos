#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

/*
La familia de funciones exec*p (execvp(), execlp(), execvpe()) buscaran el nombre de la funcion
a ejecutar en el PATH
A execvp() podemos pasarle el nombre de un programa que se encuentre en el PATH, o pasale el 
directorio donde se encuentre lo que queremos ejecutar.


TO DO:
Checkeo de error en el malloc?
*/

int main(){
    int index = 0, err = 0;
    char buff[1000], *args[1000], *token, *fun;
    pid_t pid;
    
    while(1){
        fgets(buff, 1000, stdin );
        token = strtok(buff, " \n"); /// separa segun espacio y \n
        fun = token;

        for(int i = 0 ; token != NULL ; i++) {
            args[i] = malloc(strlen(token)+1);
            args[i] = token;
            token = strtok(NULL, " \n");
            args[i+1] = NULL; /// el ultimo argumento tiene que ser NULL. 
        }

        pid = fork();
        switch (pid) {
            case -1:
                fprintf(stderr, "Fork error\n");
                break;
            case 0: //Child
                err = execvp(fun, args); //Busca la funcion "fun" en el path y la ejecuta con los argumentos args[]
                if (err != 0) {
                    printf("Orden no encontrada\n");
                    err = 0;
                }
                break;
            default: //Parent
                wait(NULL);
                break;
        }
    }
    
    return 0;
}
