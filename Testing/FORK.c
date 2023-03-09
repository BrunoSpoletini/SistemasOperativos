#include <stdio.h>
#include <unistd.h>

    

int main(){
 
    printf("Soy padre tengo pid %d\n", getpid() );

    /// un open aca devuelve  un file desciptor
    /// estos se copian al hijo (queda abierto en los dos.)

    /// la memoria se copia lazy (se pasan punteros) (inchequeable).


    int valor = 2; /// esta variable tiene la misma direccion en ambos procesos
    /// porque es la direccion virtual, pero son dos vairbales distintas.

    int ret;
    ret = fork(); /// en ret queda el pid del hijo (si sos padre) o 0 si sos hijo.

    if( ret == -1 ){ //Error
        printf("Hubo error, no hay memoria.\n");
    }
    if( ret == 0 ){ //Hijo

        sleep(1);

        printf("valor vale %d\n",valor);
        //valor = 1;

        printf("Hola soy el hijo.\n");
        printf("Soy hijo tengo pid %d\n", getpid() );
    }
    if(ret != 0){ //Padre
        valor = 3;
        printf("valor vale %d\n",valor);

        printf("Hola soy el padre.\n");
        printf("El pid de mi hijo es %d\n", ret );
    }

    return 0;
}