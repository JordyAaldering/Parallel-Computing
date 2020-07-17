#include "Util.h"
#include <math.h>

#define N 1000
#define HEAT 100.0
#define EPS 0.1

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

void run(size_t n, double heat, double eps) {
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

    Util::PrintInfo(n, heat, eps, iterations);
}

int main(int argc, char** argv) {
    run(N, HEAT, EPS);
    return 0;
}
