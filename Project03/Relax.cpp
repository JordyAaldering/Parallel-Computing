#include "Util.h"
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <time.h>

#define N 1000
#define HEAT 400.0
#define EPS 0.05

void Diffuse(double* in, double* out, size_t n, size_t i, size_t j) {
    out[i * n + j] = 0.25 * in[i * n + j] // center
        + 0.250 * in[(i - 1) * n + j]     // upper
        + 0.125 * in[(i + 1) * n + j]     // lower
        + 0.175 * in[i * n + (j - 1)]     // left
        + 0.200 * in[i * n + (j + 1)];    // right
}

/// <summary>
/// Individual step of the 5-point stencil.
/// Computes values in matrix 'out' from those in matrix 'in'.
/// </summary>
/// <param name="in">A matrix size 'n by n'.</param>
/// <param name="out">A matrix size 'n by n'.</param>
bool Relax(double* in, double* out, size_t n, double eps) {
    bool stable = true;
    for (size_t i = 1; i < n - 1; i++) {
        for (size_t j = 1; j < n - 1; j++) {
            Diffuse(in, out, n, i, j);
            if (stable && fabs(in[i * n + j] - out[i * n + j]) > eps) {
                stable = false;
            }
        }
    }

    return stable;
}

void run(std::ofstream& file, size_t n, double heat, double eps) {
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
    Util::PrintInfo(n, heat, eps, iterations, start, end);
    Util::WriteInfo(file, n, iterations, start, end);
}

int main(int argc, char** argv) {
    std::ofstream file;
    std::string filename = "Evaluation/relax.csv";
    
    file.open(filename, std::fstream::app);
    if (!file.is_open()) {
        printf("Could not open file %s.", filename);
        return 1;
    }

    for (int i = 1; i < 10; i++) {
        run(file, i * N, HEAT, EPS);
        printf("\n");
    }

    file.close();
    return 0;
}
