#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN
#include <sys/wait.h> //Para usar wait
#include <sys/socket.h> /// socket
#include <string.h> // strtok
#include <sys/un.h> /// para una struct
#include "socket_server_configuration.h"
#define CLI_NAME "./Cliente"


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
    int client_socket;
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0); /// 

    struct sockaddr_un cli_name;
    cli_name.sun_family = AF_UNIX;
    strncpy( cli_name.sun_path , CLI_NAME, sizeof( cli_name.sun_path ) );

    /// asignar nombre al socket.

    bind(client_socket, (struct sockaddr_un *) &cli_name, sizeof( cli_name ) );

    char buff[100];

    recvfrom(client_socket , buff , 100 , 0 , (struct sockaddr_un *) &cli_name, sizeof( cli_name ) ); // a lo ejor es l reveves

    printf("Nos llego %s\n", buff);


    return 0;
}