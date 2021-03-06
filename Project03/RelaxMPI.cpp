#include "Shared.h"
#include <mpi.h>

/// <summary>Prints information about the state of the program.</summary>
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

/// <summary>Calculates the size of this process' matrix.</summary>
/// <param name="rank">The rank of the current process.</param>
/// <param name="worldSize">The total number of processes.</param>
/// <param name="n">The width of the matrix.</param>
/// <returns>The size of this process' matrix.</returns>
static size_t GetArraySize(int rank, int worldSize, int n) {
    size_t arraySize = (n / worldSize) * n;
    if (rank != 0) { // share top row
        arraySize += n;
    }
    if (rank != worldSize - 1) { // share bottom row
        arraySize += n;
    } else { // add remainder
        arraySize += (n % worldSize) * n;
    }

    return arraySize;
}

/// <summary>Individual step of the 5-point stencil.</summary>
/// <param name="in">The original matrix.</param>
/// <param name="out">The resulting matrix.</param>
/// <param name="n">The width of the matrix.</param>
/// <param name="arraySize">The size of this process' matrix.</param>
/// <param name="eps">The epsilon value.</param>
/// <returns>Whether the resulting matrix is stable.</returns>
static bool Relax(double* in, double* out, size_t n, size_t arraySize, double eps) {
    bool stable = true;
    for (size_t y = 1; y < arraySize / n - 1; y++) {
        for (size_t x = 1; x < n - 1; x++) {
            size_t index = x + y * n;
            Shared::Diffuse(in, out, n, index);
            if (stable && fabs(in[index] - out[index]) > eps) {
                stable = false;
            }
        }
    }

    return stable;
}

/// <summary>Updates neighbouring processes by telling them about overlapping values with this process' matrix.
/// Then this process updates its matrix with the data received from its neighbours.</summary>
/// <param name="rank">The rank of the current process.</param>
/// <param name="worldSize">The total number of processes.</param>
/// <param name="n">The width of the matrix.</param>
/// <param name="arraySize">The size of this process' matrix.</param>
/// <param name="out">The resulting matrix.</param>
static void UpdateNeighbours(int rank, int worldSize, size_t n, size_t arraySize, double* out) {
    if (rank % 2 == 0) {
        if (rank < worldSize - 1) {
            MPI_Send(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank > 0) {
            MPI_Send(&out[0], n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
            MPI_Recv(&out[0], n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (rank > 0) {
            MPI_Recv(&out[0], n, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&out[0], n, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        }
        if (rank < worldSize - 1) {
            MPI_Recv(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&out[arraySize - n - 1], n, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
        }
    }
}

static void Run(std::ofstream& file, size_t n, double heat, double eps) {
    double start = MPI_Wtime();

    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    size_t arraySize = GetArraySize(rank, worldSize, n);

    int iterations = 1;
    double* in = Shared::CreateMatrix(arraySize, rank == 0 ? n / 2 : -1, heat);
    double* out = Shared::CreateMatrix(arraySize, rank == 0 ? n / 2 : -1, heat);
    double* tmp;

    bool local_stable, global_stable;
    while (true) {
        local_stable = Relax(in, out, n, arraySize, eps);
        MPI_Allreduce(&local_stable, &global_stable, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        if (global_stable) { // only when every process is stable we can stop
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

int main(int argc, char** argv) {
    std::ofstream file = Shared::OpenFile("mpi");
    MPI_Init(&argc, &argv);

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
