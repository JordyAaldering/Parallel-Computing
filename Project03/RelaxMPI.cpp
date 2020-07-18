#include "Shared.h"
#include <mpi.h>

/// <summary> Print information about the program. </summary>
static void PrintBlock(int rank, int worldSize, int arraySize, int n, double heat, double eps, int iterations, double start, double end) {
    printf("Rank      : %d\n", rank);
    printf("World     : %d\n", worldSize);
    printf("N         : %d\n", n);
    printf("Block     : %d\n", arraySize);
    printf("Size      : %dMB\n", (int)(arraySize * arraySize * sizeof(double) / (1024 * 1024)));
    printf("Heat      : %f\n", heat);
    printf("Epsilon   : %f\n", eps);
    printf("Iterations: %d\n", iterations);
    printf("Time      : %dms\n", (int)((end - start) * 1000.0));
    printf("\n");
}

static size_t GetArraySize(int rank, int worldSize, int n) {
    size_t arraySize = (n / worldSize) * n;
    if (rank != 0) { // share top row
        arraySize += n;
    }
    if (rank != worldSize - 1) { // share bottom row
        arraySize += n;
    }
    else { // add remainder
        arraySize += (n % worldSize) * n;
    }

    return arraySize;
}

static bool Relax(double* in, double* out, size_t n, size_t arraySize, double eps) {
    bool stable = true;
    for (size_t i = n + 1; i < arraySize - n - n; i += 3) {
        for (size_t r = 0; r < n - 1; r++, i++) {
            Shared::Diffuse(in, out, n, i);
            if (stable && fabs(in[i] - out[i]) > eps) {
                stable = false;
            }
        }
    }

    return stable;
}

static void UpdateNeighbours(int rank, int worldSize, size_t n, size_t arraySize, double* out) {
    if (rank % 2 == 0) {
        if (rank < worldSize - 1) {
            MPI_Send(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank > 0) {
            MPI_Recv(&out[0], n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&out[0], n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
        }
    } else {
        if (rank > 0) {
            MPI_Recv(&out[0], n, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&out[0], n, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        }
        if (rank < worldSize - 1) {
            MPI_Send(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
            MPI_Recv(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
}

static void Run(std::ofstream& file, size_t n, double heat, double eps) {
    double start = MPI_Wtime();

    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    size_t arraySize = GetArraySize(rank, worldSize, n);

    double* in = Shared::CreateMatrix(arraySize, rank == 0 ? n / 2 : -1, heat);
    double* out = Shared::CreateMatrix(arraySize, rank == 0 ? n / 2 : -1, heat);
    double* tmp;
    int iterations = 1;

    bool global_stable;
    while (true) {
        bool local_stable = Relax(in, out, n, arraySize, eps);
        MPI_Allreduce(&local_stable, &global_stable, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        if (global_stable) {
            break;
        }

        UpdateNeighbours(rank, worldSize, n, arraySize, out);

        tmp = in;
        in = out;
        out = tmp;
        iterations++;
    }

    double end = MPI_Wtime();
    free(in);
    free(out);

    Shared::WriteInfo(file, n, iterations, (int)((end - start) * 1000.0), worldSize);
    PrintBlock(rank, worldSize, arraySize, n, heat, eps, iterations, start, end);
}

int main2(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    std::ofstream file = Shared::OpenFile("mpi");

    for (int i = 1; i <= STEPS; i++) {
        for (int r = 0; r < REPEATS; r++) {
            Run(file, i * N, HEAT, EPS);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    file.close();
    return 0;
}
