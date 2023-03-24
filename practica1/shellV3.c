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
Checkeo de fallo de exec
Checkeo de fallo de dup
*/
void freeArr(char **args){
    int i = 0;
    while(args[i] != NULL){
        free(args[i]);
        i++;
    }
}

void runP(char** args){
    int err = 0, tempFD0, tempFD1, fileFD, red = 0, i = 0, procesos = 0, pipeFD[2], cont=0;
    char **filename, *proc[256][32];
    pid_t pid;

    while( (args[i] != NULL) && (red == 0) ){
        if ( strcmp(args[i], ">" ) == 0 ){
            filename = &args[i+1];
            proc[procesos][cont]=NULL;
            fileFD = open(filename[0], O_RDWR | O_CREAT | O_APPEND, 0666);
            dup2(fileFD, 1);
            red = 1;    
        } else if ( strcmp(args[i], "|") == 0 ){
            proc[procesos][cont]=NULL;
            procesos++;
            cont = 0;
        } else {
            proc[procesos][cont] = malloc(sizeof(args[i]));
            strcpy (proc[procesos][cont] , args[i]);
            cont++;
        }
        i++;
    }
    proc[procesos][cont]=NULL; //Para que no tome los argumentos que quedan buffereados
    procesos++;

    if (procesos == 1){
        execvp(proc[0][0], proc[0]);
    } else {
        tempFD0 = dup(0);
        tempFD1 = dup(1);
        for(int j=0; j < procesos ; j++){  
            pipe(pipeFD);
            pid = fork();
            switch (pid) {
                case -1:
                    fprintf(stderr, "Fork error\n");
                    exit(0);
                    break;
                case 0: //Child
                    close(pipeFD[0]);
                    //El proceso de la derecha de todo escribe por la salida estandar
                    if (j != (procesos-1)){
                        dup2(pipeFD[1], 1);
                    } else {
                        dup2(tempFD1, 1);//Solo por claridad de codigo
                    }
                    execvp(proc[j][0], proc[j]);
                    break;
                default: //Parent
                    close(pipeFD[1]);
                    dup2(pipeFD[0], 0);
                    break;
            }
        }
        dup2(tempFD0,0);
        dup2(tempFD1,1);
    }

    // if (err != 0) {
    //     printf("Orden no encontrada\n");
    //     err = 0;
    // }
}

int main(){
    int index = 0, err = 0;
    char buff[1000], **args, *token;
    args = malloc(sizeof(char*)*1000);
    pid_t pid;
    
    while(1){
        fgets(buff, 1000, stdin );
        token = strtok(buff, " \n"); /// separa segun espacio y \n

        for(int i = 0 ; token != NULL ; i++) { //Parseo del string ingresado en argumentos
            args[i] = malloc(strlen(token)+1);
            strcpy(args[i], token);
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
                freeArr(args);
                break;
        }
    }
    free(args);
    return 0;
}
