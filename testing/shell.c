#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok


int main(){
    char buff[1000];
    fgets(buff, 1000, stdin );


    char *token;
    token = strtok(buff, " \n"); /// separa segun espacio y \n
    
    char *path = malloc( 1000 ); /// 1000 caracteres
    strcat( path , "/bin/" ) ;
    strcat( path , token ) ;
    
    char *args[100];
    int index = 0;
    while(token != NULL){
	args[index] = malloc(strlen(token)+1);
	strcpy(args[index],token); 
    token = strtok(NULL, " \n");
    index++;
    args[index] = NULL; /// el ultimo argumento tiene que ser NULL.
    }
    
    execv(path,args);
    
    return 0;
}
