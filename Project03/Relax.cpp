#include "Util.h"
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <time.h>

/// <summary>
/// Individual step of the 5-point stencil.
/// Computes values in matrix 'out' from those in matrix 'in'.
/// </summary>
/// <param name="in">A matrix size 'n by n'.</param>
/// <param name="out">A matrix size 'n by n'.</param>
bool Relax(double* in, double* out, size_t n, double eps) {
    bool stable = true;
    for (size_t i = n + 1; i < n * (n - 2); i += 3) {
        for (size_t r = 0; r < n - 1; r++, i++) {
            Util::Diffuse(in, out, n, i);
            if (stable && fabs(in[i] - out[i]) > eps) {
                stable = false;
            }
        }
    }

    return stable;
}

void Run(std::ofstream& file, size_t n, double heat, double eps) {
    clock_t start = clock();

    double* a = Util::CreateMatrix(n, heat);
    double* b = Util::CreateMatrix(n, heat);
    double* tmp;
    int iterations = 1;

    while (!Relax(a, b, n, eps)) {
        tmp = a;
        a = b;
        b = tmp;
        iterations++;
    }

    clock_t end = clock();
    free(a);
    free(b);

    Util::WriteInfo(file, n, iterations, start, end);
    Util::PrintInfo(n, heat, eps, iterations, start, end);
}

int main2() {
    std::ofstream file;
    file.open("Evaluation/relax.csv");
    if (!file.is_open()) {
        printf("Could not open file.");
        return 1;
    }

    Run(file, N, HEAT, EPS);
    file.close();
    return 0;

    for (int i = 44; i <= 50; i++) {
        for (int r = 0; r < 10; r++) {
            Run(file, i * N, HEAT, EPS);
        }
    }

    file.close();
    return 0;
}
