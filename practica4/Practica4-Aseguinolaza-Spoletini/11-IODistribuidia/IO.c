#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define tambuf 4*32 /// Hasta 32 procesadores

int main(int argc, char **argv){
    
    int pid, npr;
    int i, numdat;
    int buf[tambuf], buf2[tambuf], modoacceso;
    MPI_File dat1;
    MPI_Status info;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);
    numdat = 4; /// 4 datos por procesador.
    
    if (pid == 0)
        for(i=0; i<npr*numdat; i++) buf[i] = i*i; /// guardamos i^2 en numdat*CantProcesadores lugares del buff
    if (pid == 0){
        modoacceso = (MPI_MODE_CREATE | MPI_MODE_WRONLY); /// el modo es crear si no existe, en modo lectura y escritura.
        MPI_File_open(MPI_COMM_SELF, "dat1", modoacceso, MPI_INFO_NULL, &dat1); /// el master crea un archivo dat1
        MPI_File_seek(dat1, 0, MPI_SEEK_SET); /// al inicio del archivo
        MPI_File_write(dat1, buf, npr*numdat, MPI_INT, &info); /// escribimos el buffer
        MPI_File_close(&dat1); /// cerramos el archivo.
        printf("\n El master escribió %d datos, desde 0 hasta %d \n\n", npr*numdat, buf[npr*numdat-1]); //// 0 hasta maximovalorBuff.
    }
    modoacceso = MPI_MODE_RDONLY;
    MPI_File_open(MPI_COMM_WORLD, "dat1", modoacceso, MPI_INFO_NULL, &dat1);
    MPI_File_seek(dat1, pid*numdat*sizeof(int), MPI_SEEK_SET);
    MPI_File_read(dat1, buf2, numdat, MPI_INT, &info);
    MPI_File_close(&dat1);
    printf(" > %d ha leido %5d %5d %5d %5d \n", pid, buf2[0], buf2[1], buf2[2], buf2[3]);
    MPI_Finalize(); 
    return 0;   
}
