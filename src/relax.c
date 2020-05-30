#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define N 1000000   // length of the vectors
#define HEAT 100.0  // heat value on the boundary
#define EPS 0.1     // convergence criterium

#define ALLOCATE(type, size) (type*)malloc(size * sizeof(type))

/**
 * initialise the values of the given vector "out" of length "n"
 */
void init(double *out, int n) {
    out[0] = HEAT;
    for (int i = 1; i < n; i++) {
        out[i] = 0;
    }
}

/**
 * individual step of the 3-point stencil
 * computes values in vector "out" from those in vector "in"
 * assuming both are of length "n"
 */
void relax(double *in, double *out) {
    for (int i = 1; i < N - 1; i++) {
        out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];
    }
}

/**
 * checks the convergence criterion:
 * true, iff for all indices i, we have |out[i] - in[i]| <= eps
 */
bool isStable(double *old, double *new) {
    bool res = true;
    for (int i = 1; i < N - 1; i++) {
        res &= fabs(old[i] - new[i]) <= EPS;
    }

    return res;
}

int main() {
    double *old, *new, *tmp;
    old = ALLOCATE(double, N);
    new = ALLOCATE(double, N);

    init(old, N);
    init(new, N);

    int iterations = 0;
    clock_t start = clock();

    do {
        tmp = old;
        old = new;
        new = tmp;

        relax(old, new);
        iterations++;
    } while (!isStable(old, new));

    clock_t end = clock();
    printf("Iterations: %d\n", iterations);
    printf("Duration: %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
