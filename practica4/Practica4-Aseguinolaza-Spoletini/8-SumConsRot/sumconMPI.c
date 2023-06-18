#include <stdio.h>
#include <mpi.h>
#include <assert.h>

/*
 -- sudo apt install libopenmpi-dev
 -- mpicc archivo.c -o salida.o
 -- mpirun -np 4 ./salida.o
 -- mpicc archivo.c -o salida.o && mpirun -np 4 ./salida.o
*/

int main(int argc, char** argv){
    int process_Rank, size_Of_Cluster, message_Item;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);

    /// hagamos la suma de los process_Ranks.

    int idx = process_Rank;
    int sig = (idx+1)%size_Of_Cluster;
    int ant = (idx-1+size_Of_Cluster)%size_Of_Cluster;
    int val = idx+1;

    MPI_Request request;

    int totsum = 0;
    MPI_Isend(&val, 1, MPI_INT, sig, 0, MPI_COMM_WORLD, &request); /// enviamos nuestro val al siguiente. (La I es para no bloqueante).
    int get;
    MPI_Recv(&get, 1, MPI_INT, ant, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /// recibimos del anterior.
    totsum += get; /// sumamos a un total

    for(int i = 0; i < size_Of_Cluster-1 ; i++){
        MPI_Isend(&get, 1, MPI_INT, sig, 0, MPI_COMM_WORLD, &request); /// enviamos el valor que nos pasaron, al siguiente.
        MPI_Recv(&get, 1, MPI_INT, ant, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /// recibimos del anterior, un nuevo valor
        totsum += get; /// sumamos a un total
    }

    assert(totsum == size_Of_Cluster*(size_Of_Cluster+1)/2 );

    MPI_Finalize();

    return 0;
}
