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

void runP(char** args){
    int err = 0, tempFD, fileFD, red = 0;
    char **args1, **args2, simb;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">" ) == 0 ){ //|| strcmp(args[i], "<" ) == 0) {
            args1 = &args[0];
            args2 = &args[i+1];
            simb = args[i][0];
            args[i] = NULL;
            red = 1;
        }
    }

    if (red == 1) {
        fileFD = open(args2[0], O_RDWR | O_CREAT | O_APPEND, 0666);
        dup2(fileFD, 1);
    } 

    err = execvp(args[0], args); //Busca la funcion "fun" en el path y la ejecuta con los argumentos args[]
    if (err != 0) {
        printf("Orden no encontrada\n");
        err = 0;
    }

    // if (red == 1) { // Como se duplican los fd con el fork, en teoria cuando muera el proceso, los fd se resolverian solos...?
    //     dup2(tempFD, 1);
    //     close(fileFD);
    //     printf("Test stdout\n");
    // }

}

void freeArr(char **args){
    int i = 0;
    while(args[i] != NULL){
        free(args[i]);
        i++;
    }
}

int main(){
    int index = 0, err = 0;
    char buff[1000], **args, *token;
    args = malloc(sizeof(char)*1000);
    pid_t pid;
    
    while(1){
        fgets(buff, 1000, stdin );
        token = strtok(buff, " \n"); /// separa segun espacio y \n

        for(int i = 0 ; token != NULL ; i++) { //Parseo del string ingresado en argumentos
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
                runP(args);
                break;
            default: //Parent
                wait(NULL);
                //freeArr(args);
                break;
        }
    }

    //free(args);
    
    return 0;
}