#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int i, rank, size;
    double x, sum = 0.0, result = 0.0, pi = 0.0;

    double start, end;
    int steps = 1E7;
    double stepSize = 1.0 / steps;

    MPI_Init(&argc, &argv);

    start = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Barrier(MPI_COMM_WORLD);

    for (i = rank; i < steps; i += size) {
        x = (i + 0.5) * stepSize;
        result += 4.0 / (1.0 + x * x);
    }

    MPI_Reduce(&result, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        pi = stepSize * result;
        end = MPI_Wtime();
        printf("Pi=%lf with size %2d in %fs\n", pi, size, end - start);
    }

    MPI_Finalize();
    return 0;
}
