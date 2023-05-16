#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv){

    int rank, size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Soy el thread %d, y ya estoy listo\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);


    if(rank == 0){
        printf("Ingresa un numerito\n");
        int c;
        scanf("%d",&c);
        MPI_Send( &c , 1 ,MPI_INT, 1, 0, MPI_COMM_WORLD);
    }else{
        int c;
        MPI_Recv(&c, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Soy el proceso %d, y me pasaron el num %d\n",rank,c);
        if(rank != size-1){
            MPI_Send( &c , 1 ,MPI_INT, rank+1, 0, MPI_COMM_WORLD);
        }
    }

    //printf("Soy el thread, %d de un total de %d\n",rank, size);
    MPI_Finalize();


    return 0;
}

/*

MPI_Send(
    void* data,
    int count,
    MPI_Datatype datatype,
    int destination,
    int tag,
    MPI_Comm communicator)

MPI_Recv(
    void* data,
    int count,
    MPI_Datatype datatype,
    int source,
    int tag,
    MPI_Comm communicator,
    MPI_Status* status)




*/

/*

son todos procesos distintos: no hay memoria compartida.

se compial con con mpicc asd.c -o pepe.o
se corre con mpirun -np 4 ./pepe.o
*/