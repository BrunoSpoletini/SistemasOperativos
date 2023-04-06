#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

void quit(char *s) {
	perror(s);
	abort();
}

int safeDup(int fd){
    int copia = dup(fd);
    if (copia == -1){
        quit("Fallo al realizar dup");
    }
    return copia;
}

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
            if (fileFD == -1)
                quit("Fallo al crear el archivo");
            if(dup2(fileFD, 1) == -1){
                quit("Fallo al realizar dup2");
            }
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
        if (execvp(proc[0][0], proc[0]) == -1)
            quit("Fallo execvp");
    } else {
        tempFD0 = dup(0);
        if (tempFD0 == -1) { quit("Fallo al realizar dup");}
        tempFD1 = dup(1);
        if (tempFD1 == -1) { quit("Fallo al realizar dup");}
        for(int j=0; j < procesos ; j++){  
            if (pipe(pipeFD) == -1)
                quit("Fallo pipe");
            pid = fork();
            switch (pid) {
                case -1:
                    quit("Fallo fork");
                    exit(0);
                    break;
                case 0: //Child
                    close(pipeFD[0]);
                    //El proceso de la derecha de todo escribe por la salida estandar
                    if (j != (procesos-1)){
                        if(dup2(pipeFD[1], 1) == -1){
                            quit("Fallo al realizar dup2");
                        }
                    } else {
                        if(dup2(tempFD1, 1) == -1){
                            quit("Fallo al realizar dup2");
                        }
                    }
                    if (execvp(proc[j][0], proc[j]))
                        quit("Fallo execvp\n");
                    break;
                default: //Parent
                    close(pipeFD[1]);
                    if(dup2(pipeFD[0], 0) == -1){
                        quit("Fallo al realizar dup2");
                    }
                    break;
            }
        }
        if (dup2(tempFD0,0) == -1) { quit("Fallo al realizar dup2"); }
        if (dup2(tempFD1,1) == -1) { quit("Fallo al realizar dup2"); }
    }
}

int main(){
    int index = 0, err = 0;
    char buff[1000], **args, *token;
    args = malloc(sizeof(char*)*1000);
    if(args == NULL)
        quit("Fallo malloc");
    pid_t pid;
    
    while(1){
        fgets(buff, 1000, stdin );
        token = strtok(buff, " \n"); /// separa segun espacio y \n

        for(int i = 0 ; token != NULL ; i++) { //Parseo del string ingresado en argumentos
            args[i] = malloc(strlen(token)+1);
            if (args[i] == NULL)
                quit("Fallo malloc");
            strcpy(args[i], token);
            token = strtok(NULL, " \n");
            args[i+1] = NULL; /// el ultimo argumento tiene que ser NULL. 
        }

        pid = fork();
        switch (pid) {
            case -1:
                quit("Fallo fork");
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
