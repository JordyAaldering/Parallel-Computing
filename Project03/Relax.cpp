#include "Shared.h"
#include <time.h>

/// <summary> Print information about the state of the program. </summary>
static void PrintMatrix(int n, double heat, double eps, int iterations, clock_t start, clock_t end) {
    printf("N         : %d\n", n);
    printf("Size      : %dMB\n", (int)(n * n * sizeof(double) / (1024 * 1024)));
    printf("Heat      : %f\n", heat);
    printf("Epsilon   : %f\n", eps);
    printf("Iterations: %d\n", iterations);
    printf("Time      : %dms\n", (int)((end - start) / (CLOCKS_PER_SEC / 1000.0)));
    printf("\n");
}

/// <summary> Individual step of the 5-point stencil. </summary>
/// <param name="in">The original matrix.</param>
/// <param name="out">The resulting matrix.</param>
/// <param name="n">The width of the matrix.</param>
/// <param name="eps">The epsilon value.</param>
/// <returns>Whether the resulting matrix is stable.</returns>
static bool Relax(double* in, double* out, size_t n, double eps) {
    bool stable = true;
    for (size_t i = n + 1; i < n * (n - 2); i += 3) {
        for (size_t r = 0; r < n - 1; r++, i++) {
            Shared::Diffuse(in, out, n, i);
            if (stable && fabs(in[i] - out[i]) > eps) {
                stable = false;
            }
        }
    }

    return stable;
}

static void Run(std::ofstream& file, size_t n, double heat, double eps) {
    clock_t start = clock();

    int iterations = 1;
    double* in = Shared::CreateMatrix(n * n, n / 2, heat);
    double* out = Shared::CreateMatrix(n * n, n / 2, heat);
    double* tmp;

    while (!Relax(in, out, n, eps)) {
        tmp = in;
        in = out;
        out = tmp;
        iterations++;
    }

    clock_t end = clock();
    free(in);
    free(out);

    Shared::WriteInfo(file, n, iterations, (int)((end - start) / (CLOCKS_PER_SEC / 1000.0)));
    PrintMatrix(n, heat, eps, iterations, start, end);
}

int main2() {
    std::ofstream file = Shared::OpenFile("relax");

    for (int i = 1; i <= STEPS; i++) {
        for (int r = 0; r < REPEATS; r++) {
            Run(file, i * N, HEAT, EPS);
        }
    }

    file.close();
    return 0;
}
