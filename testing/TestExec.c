#include <stdio.h>
#include <unistd.h>
int main(){

    printf("Hola soy A y tengo PID = %d\n",getpid() );

    execl("./a", (char*) NULL);

    return 0;
}