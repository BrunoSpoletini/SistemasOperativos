#include <stdbool.h>
#include <stdio.h>

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MAX_THREADS 101
int TURN[MAX_THREADS];
bool CHOOSE[MAX_THREADS];

int Nthreads;

/*
Queremos una libreria a la que le pases una cantidad de threads, y una funcion, y haga de
scheduler entre esos threads mientras corren dicha funcion
*/
void init(int threads);
void lock(int index);
void unlock(int index);