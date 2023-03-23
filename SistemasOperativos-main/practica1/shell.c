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
De momento, soporta un unico redireccionamiento de saldia estandar a la vez.
Checkeo de error en el malloc?
*/
void freeArr(char **args){
    int i = 0;
    while(args[i] != NULL){
        free(args[i]);
        i++;
    }
}

void runP(char** args){
    int err = 0, tempFD0, tempFD1, fileFD, red = 0, i = 0, procesos = 0, pos[256], pipeFD[2];
    char **args1, **filename, simb, ***proc;
    pid_t pid;


    while( (args[i] != NULL) && (red == 0) ){
        /*if ( strcmp(args[i], ">" ) == 0 ){
            filename = &args[i+1];
            args[i] = NULL;
            fileFD = open(filename[0], O_RDWR | O_CREAT | O_APPEND, 0666);
            dup2(fileFD, 1);
            red = 1;    
        }*/
        if ( i == 0 ) {
            pos[procesos] = i;
            procesos++;
        }
        if ( strcmp(args[i], "|") == 0 ){
            pos[procesos] = i+1;
            procesos++;
            args[i] = NULL;
        }
        i++;
    }

    if (procesos == 1){
        execvp(args[0], args);
    } else {
        tempFD0 = dup(0);
        tempFD1 = dup(1);
        
        for (int j = procesos-1; j >= 0; j--){ // creamos los procesos en orden inverso.

            pipe(pipeFD);

            dup2(pipeFD[1], 1); //Esta parte hay que corregirla y posib. reubicarla en el child.
            //pipe[0] = read
            //pipe[1] = write
            if (j != 0){
                dup2(pipeFD[0], 0);
            }


            pid = fork();
            /// forkeamos y despues el hijo nos pasa su fd[0].
            /// este valor (fdsig) se lo tenemos que pasar al siguiente (anterior) hijo que creemos.
            /// y en este nuevo hijo, hacemos dup(fdsig, 1). entonces el stdout de este hijo pasa a ser stdin del siguiente.
            /// 
            switch (pid) {
                case -1:
                    fprintf(stderr, "Fork error\n");
                    exit(0);
                    break;
                case 0: //Child
                    close(pipeFD[1]);
                    char buff[100];
                    read(pipeFD[0],buff,100);
                    int FD0sig = atoi(buff); 

                    //pipe[0] = read
                    //pipe[1] = write
                    /// FD 0 = STDIN
                    /// FD 1 = STDOUT

                    /// el pŕimer proceso se le cambia el stdout al stdin del segundo proceso.
                    /// o sea un dup( PIPEFDSEGUNDO[0] ,1) en el hijo 1.

                    /// salvo el ultimo, que hay que dup2(tempFD1, 1) 
                    dup2( pipeFD[0] , 1 ); /// el std pasa a ir 
                    if (j == (procesos-1)){
                        dup2(tempFD1, 1);
                    }
                    /// redireccionar:

                    execvp(args[pos[j]], &args[pos[j]]); /// los fds se mantienen.
                    break;
                default: //Parent

                    write()
                    if(){
                        close(pipeFD[1]);
                    }
                    if(){
                        close(pipeFD[0]);
                    }
                    wait(NULL);
                    break;
            }
        }

    }



    // err = execvp(args[0], args); //Busca la funcion "fun" en el path y la ejecuta con los argumentos args[]
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
