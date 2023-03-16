#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(){
    /*
    int r;
    printf("Estoy en el proceso nuevo!\n");
    printf("Pid Proceso nuevo: %d\n", getpid());
    
    getchar(); //Para revisar el estado del proceso antes de pedir memoria nueva.

    sleep(100);
    */


    // Ej 1) h)
    int r;
    write(1, "Salida por stdout (1)!\n", 23);
    write(2, "Salida por stderr (2)!\n", 23); 
    
    return 0;
}
