#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h> //Para usar OPEN

#include <sys/wait.h> //Para usar wait

#include <string.h> // strtok

#include <signal.h>

int denom = 0;


/*

SIG_DFL	default signal handling
SIG_IGN	signal is ignored
*/


void handler(int s) { 
    printf("ouch!\n"); 
    denom = 1; 
    exit(0); //No esta definido el comportamiento si no se hace un exit/longjump desde el handler
}

int main() {
    int r;
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;

    sigaction(SIGFPE, &sa, NULL);
    r = 1 / denom;
    printf("r = %d\n", r);
    return 0;
}

void INThandler(int sig) {
    char c;
    signal(SIGSTOP, SIG_DFL);
    printf("OUCH, did you hit Ctrl-C?\n"
    "Do you really want to quit? [y/n] ");
    
    c = getchar();
    if (c == 'y' || c == 'Y')
        exit(0);
    else
        signal(SIGCONT, SIG_DFL);
    getchar();
}

int main2() {

    signal(SIGINT, INThandler);
    while (1)
        sleep(10);
    return 0;
}

