#include "Util.h"
#include <mpi.h>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <time.h>

void PrintInfo(int rank, int worldSize, int arraySize, int n, double heat, double eps, int iterations, double start, double end) {
    printf("Rank      : %d\n", rank);
    printf("World     : %d\n", worldSize);
    printf("N         : %d\n", n);
    printf("Block     : %d\n", arraySize);
    printf("Size      : %dMB\n", (int)(n * n * sizeof(double) / (1024 * 1024)));
    printf("Heat      : %f\n", heat);
    printf("Epsilon   : %f\n", eps);
    printf("Iterations: %d\n", iterations);
    printf("Time      : %dms\n", (int)((end - start) * 1000.0));
    printf("\n");
}

double* CreateBlock(size_t n, size_t arraySize, int rank, double heat) {
    double* m = (double*)calloc(arraySize, sizeof(double));
    if (rank == 0) { // set center of top row
        m[n / 2] = heat;
    }
    return m;
}

bool Relax(double* in, double* out, size_t n, size_t arraySize, double eps) {
    bool stable = true;
    for (size_t i = n + 1; i < arraySize - n - n + 2; i += 3) {
        for (size_t r = 0; r < n - 1; r++, i++) {
            Util::Diffuse(in, out, n, i);
            if (stable && fabs(in[i] - out[i]) > eps) {
                // printf("In[%d]: %f, out[%d]: %f\n", i, in[i], i, out[i]);
                stable = false;
            }
        }
    }

    // if (stable) printf("Stable\n");
    return stable;
}

void SendRecvBottomRow(int rank, int worldSize, size_t n, size_t arraySize, double* out) {
    if (rank % 2 == 0) {
        if (rank < worldSize - 1) {
            // printf("Process %d sending bottom row.\n", rank);
            MPI_Send(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
        if (rank > 0) {
            MPI_Recv(&out[0], n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Process %d received bottom row.\n", rank);
        }
    } else {
        if (rank > 0) {
            MPI_Recv(&out[0], n, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Process %d received bottom row.\n", rank);
        }
        if (rank < worldSize - 1) {
            // printf("Process %d sending bottom row.\n", rank);
            MPI_Send(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
        }
    }
}

void SendRecvTopRow(int rank, int worldSize, size_t n, size_t arraySize, double* out) {
    if (rank % 2 == 0) {
        if (rank > 0) {
            // printf("Process %d sending top row.\n", rank);
            MPI_Send(&out[0], n, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        }
        if (rank < worldSize - 1) {
            MPI_Recv(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Process %d received top row.\n", rank);
        }
    } else {
        if (rank < worldSize - 1) {
            MPI_Recv(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Process %d received top row.\n", rank);
        }
        if (rank > 0) {
            // printf("Process %d sending top row.\n", rank);
            MPI_Send(&out[0], n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
        }
    }
}

void Run(int argc, char** argv, std::ofstream& file, size_t n, double heat, double eps) {
    MPI_Init(&argc, &argv);
    double start = MPI_Wtime();

    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    size_t arraySize = (n / worldSize) * n;
    if (rank != 0) { // share top row
        arraySize += n;
    }
    if (rank != worldSize - 1) { // share bottom row
        arraySize += n;
    } else { // add remainder
        arraySize += (n % worldSize) * n;
    }

    double* in = CreateBlock(n, arraySize, rank, heat);
    double* out = CreateBlock(n, arraySize, rank, heat);
    double* tmp;
    int iterations = 1;

    bool global_stable;
    while (true) {
        bool local_stable = Relax(in, out, n, arraySize, eps);

        // continue as long as any process is not stable
        MPI_Allreduce(&local_stable, &global_stable, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        if (global_stable) {
            break;
        }

        SendRecvBottomRow(rank, worldSize, n, arraySize, out);
        SendRecvTopRow(rank, worldSize, n, arraySize, out);

        tmp = in;
        in = out;
        out = tmp;
        iterations++;
    }

    double end = MPI_Wtime();
    MPI_Finalize();

    PrintInfo(rank, worldSize, arraySize, n, heat, eps, iterations, start, end);
}

int main(int argc, char** argv) {
    std::ofstream file;
    file.open("Evaluation/mpi.csv", std::fstream::app);
    if (!file.is_open()) {
        printf("Could not open file.");
        return 1;
    }

    for (int i = 1; i <= 1; i++) {
        for (int r = 0; r < 1; r++) {
            Run(argc, argv, file, i * N, HEAT, EPS);
        }
    }

    file.close();
    return 0;
}