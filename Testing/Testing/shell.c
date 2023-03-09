#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok


int main(){
    char buff[1000];
    fgets(buff, 1000, stdin );


    char *token;
    token = strtok(buff, " ");

    char comando[1000];
    
    strcpy(comando, token);

    char **argumentos;
    argumentos = malloc(sizeof(char)* 100);//100 argumentos top

    for(int i=0; token!=NULL; i++){
        token = strtok(NULL, " ");
        argumentos[i] = malloc(sizeof(token));
        strcpy(argumentos[i], token);
        argumentos[i+1] = NULL;
   }
   printf("hola\n");

    char ruta[1000];
    strcpy(ruta, "/usr/bin");
    strcpy(ruta, comando);
    execvp( ruta, argumentos );

    //strtok();

    // shell proceso parent
    // parseo de input
    // fork por comando, shell en wait



    return 0;
}