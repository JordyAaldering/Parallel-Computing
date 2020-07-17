#include <stdio.h>
#include <mpi.h>

// mpiexec -n 8 "D:\Documents\Parallel-Computing\Project03\Debug\Project03.exe"
int main2(int argc, char** argv) {
    int rank, size;
    double start, end;

    MPI_Init(&argc, &argv);
    start = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Rank %d of %d processors.\n", rank, size);

    end = MPI_Wtime();
    MPI_Finalize();

    if (rank == 0) {
        printf("Finished in %fs.\n", end - start);
    }

    return 0;
}
