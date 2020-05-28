#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define N 1000000  // length of the vectors
#define HEAT 100.0  // heat value on the boundary
#define EPS 0.1  // convergence criterium

/**
 * allocate a vector of length "n"
 */
double* allocVector(int n) {
    double *v;
    v = (double*) malloc(n * sizeof(double));
    return v;
}

/**
 * initialise the values of the given vector "out" of length "n"
 */
void init(double *out, int n) {
    memset(out, 0, n * sizeof(double));
    out[0] = HEAT;
}

/**
 * individual step of the 3-point stencil
 * computes values in vector "out" from those in vector "in"
 * assuming both are of length "n"
 */
bool relax(double *in, double *out, int n, double eps) {
    bool stable = true;
    for (int i = 1; i < n - 1; i++) {
        out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];

        if (stable && fabs(in[i] - out[i]) > eps) {
            stable = false;
        }
    }

    return stable;
}

int main() {
    bool stable = true;
    int iterations = 0;
    double *old = allocVector(N);
    double *new = allocVector(N);
    double *tmp;

    init(old, N);
    init(new, N);

    printf("size   : %d M (%d MB)\n", N / 1000000, (int) (N * sizeof(double) / (1024 * 1024)));
    printf("heat   : %f\n", HEAT);
    printf("epsilon: %f\n", EPS);

    clock_t start = clock();

    do {
        tmp = old;
        old = new;
        new = tmp;

        stable = relax(old, new, N, EPS);
        iterations++;
    } while (!stable);

    clock_t end = clock();
    printf("Number of iterations: %d\n", iterations);
    printf("Done in %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
