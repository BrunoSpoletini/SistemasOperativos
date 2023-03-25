#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

#include <signal.h>
/*
Luego del fork, el padre env´ıa una se˜nal SIGUSR1 al hijo y entra en un loop infinito. El hijo espera en
un loop infinito. Ambos (padre e hijo) cuando reciven una se˜nal SIGUSR1 responden lo mismo. (el padre
al hijo y el hijo al padre). ¿Qu´e pasa si cambiamos los loops infinitos por pause()?. Usar las funciones
signal() y signalaction().
*/
void handlerUsrPadre(int s){
    printf("Recibido hijo!\n");
    exit(0);
}
void handlerUsrHijo(int s){
    printf("Recibido padre!\n");
    exit(0);
}
int main(){

    pid_t pid = fork();

    switch (pid)
    {
    case -1:
        printf("Fallo el fork\n");
        exit(0);
        break;
    case 0: //child

        signal(SIGUSR1, handlerUsrHijo);
        while (1){;}
        
    default: //parent
        //signal(SIGUSR1, SIG_DFL);
        sleep(1);
        kill(pid, SIGUSR1);
        signal(SIGUSR1, handlerUsrPadre);
        while (1){;}
        break;
    }



    return 0;
}