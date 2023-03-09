#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait


int abrirArchivo();
int testWait();

int main(){
    //forkDoble();
    //abrirArchivo();
    testWait();
    return 0;
}

int testWait(){

    pid_t ret = fork();

    if ( ret == 0 ) { //Child
        for (int i = 0; i < 10; i++){
            printf("%d", i);
        }
        exit(2);
    } else { //Parent
        int retval;
        wait(&retval);
        printf("\nFinalizado con valor = %d\n",retval);
    }


    return 0;
}

int abrirArchivo(){

    int fd = open("text.txt", O_RDWR);
    printf("File descriptor: %d\n", fd);

    char buf[1000];
    buf[42] = '\0';
    int bytesRead = read(fd, buf, 42);

    printf("Buffer:\n%s\n", buf);

    return fd;
}

int forkDoble(){

    pid_t childp1 = fork();
    pid_t childp2 = fork();

    /*if( ret == -1 ){ //Error
        printf("Hubo error, no hay memoria.\n");
    } */

    if ( childp1 == 0){
        
        if (childp2 == 0){
            printf("Hijo 2 PID: %d \n", getpid());
        } else {
            printf("Padre 2 PID: %d \n", getpid());
        }
    }
    else {
        if (childp2 == 0){
            printf("Hijo 1 PID: %d \n", getpid());
        } else {
            printf("Padre 1 (Original) PID: %d \n", getpid());
        }
    }

    return 0;
}