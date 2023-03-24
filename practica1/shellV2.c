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
    int err = 0, tempFD0, tempFD1, fileFD, red = 0, i = 0, procesos = 0, pos[256], pipeFD[2], cont=0;
    char **args1, **filename, simb, *proc[256][32];
    pid_t pid;


    while( (args[i] != NULL) && (red == 0) ){
        /*if ( strcmp(args[i], ">" ) == 0 ){
            filename = &args[i+1];
            args[i] = NULL;
            fileFD = open(filename[0], O_RDWR | O_CREAT | O_APPEND, 0666);
            dup2(fileFD, 1);
            red = 1;    
        }*/
        // if ( i == 0 ) {
        //     pos[procesos] = i;
        //     procesos++;
        // }

        

        if ( strcmp(args[i], "|") == 0 ){
            procesos++;
            cont = 0;
            // pos[procesos] = i+1;
            // procesos++;
            // args[i] = NULL;
            
        } else {
            proc[procesos][cont] = malloc(sizeof(args[i]));
            strcpy (proc[procesos][cont] , args[i]);
            cont++;
        }
        i++;
    }
    procesos++;

    if (procesos == 1){
        execvp(proc[0][0], proc[0]);
    } else {
        tempFD0 = dup(0);
        tempFD1 = dup(1);

        
        for (int j = 0; j < procesos; j++){
            
            pipe(pipeFD);

            int fdin = j == 0 ? tempFD0 : pipeFD[0] ;
            int fdout = j == (procesos -1) ? tempFD1 : pipeFD[1];

            pid = fork();
            switch (pid) {
                case -1:
                    fprintf(stderr, "Fork error\n");
                    exit(0);
                    break;
                case 0: //Child
                    close(pipeFD[1]);
                    //dup2(fdin, 0);
                    dup2(fdin, 0);

                    execvp(proc[j][0], proc[j]);
                    break;
                default: //Parent

                    dup2(fdout,0);
                     if (j > 0) {
                        close(pipeFD[0]);
                    }
                    //wait(NULL);
                    break;
            }
        }
        dup2(tempFD0,0);

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
