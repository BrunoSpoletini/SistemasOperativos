#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

int main(){
    int err = 0, tempFD, fileFD, red = 0;
    char **args1, **args2, simb;
int idk;

    fileFD = open("dup.txt", O_RDWR | O_CREAT );
    tempFD = dup(1);
    dup2( fileFD, 1); // stdout (1) ahora apunta a fileFD
    
    write(1, "Hola mundo!", 11 );
    

    // err = execvp(args[0], args); //Busca la funcion "fun" en el path y la ejecuta con los argumentos args[]
    // if (err != 0) {
    //     printf("Orden no encontrada\n");
    //     err = 0;
    // }

    close(fileFD);
    
    dup2(tempFD, 1);

    printf("Test stdout\n");
    return 0;
}