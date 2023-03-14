#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>

/// estoy en windows esto no corre.

int main() {

    int pid = fork();

    if(pid == 0){
        /// soy child
        exit(0);

    }else{
        /// soy padre.
        while(1){
            sleep(1000);
        }

    }


    return 0;

}