#include <stdio.h>
#include <unistd.h>
int main(){

    printf("Hola soy A y tengo PID = %d\n",getpid() );

    execl("./B",NULL);

    //printf("Hola soy ls y tengo PID = %d\n",getpid() );


    return 0;
}