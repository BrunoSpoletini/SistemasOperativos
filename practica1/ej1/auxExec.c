#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(){
    int r;
    printf("Estoy en el proceso nuevo!\n");
    printf("Pid Proceso nuevo: %d\n", getpid());
    
    getchar(); //Para revisar el estado del proceso antes de pedir memoria nueva.

    sleep(100);

    return 0;
}
