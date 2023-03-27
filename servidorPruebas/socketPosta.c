#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <sys/socket.h>

#include <string.h> // strtok

int main(){
    //int socket(int dominio, int type, int protocol)
    /*
    Dominio: local/red
        Local:
        ( AF_UNIX / AF_LOCAL )
        Red:
        ( AF_INET/AF_INET6 )
    Tipo: Datagrama/Stream
        UDP:
        (SOCL_DGRAM) no confiable y no orientado a la conexion
        TCP:
        (SOCK_STREAM) confiable y orientado a la conexion
    Protocolo: ip..? (0)

    int socketpair(int domain, int type, int protocol, int sv[2]);
    */
    int var, fd[2];
    char buff[1000];
    var = socketpair(AF_UNIX, SOCK_STREAM, 0,fd);

    pid_t pid = fork();

    switch (pid)
    {
    case 0: //Child
        close(fd[0]);
        recv(fd[1],buff,100,0);
        printf("%s",buff);
        break;
    
    default: //Parent
        close(fd[1]);
        send(fd[0],"Msg to child\n",13,0  );

        wait(NULL);
        break;
    }

    return 0;
}