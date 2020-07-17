#include <fstream>
#include <iostream>
#include <stdio.h>
#include <mpi.h>

void writeToFile() {
    std::ofstream file;
    file.open("Evaluation/data.csv", std::fstream::app);
    if (!file.is_open()) {
        printf("Could not open file.");
        return;
    }

    file << "text" << std::endl;
    file.close();
}

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

    writeToFile();

    return 0;
}
