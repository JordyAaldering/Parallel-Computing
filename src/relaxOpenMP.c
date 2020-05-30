#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define N 1000000   // length of the vectors
#define HEAT 100.0  // heat value on the boundary
#define EPS 0.1     // convergence criterium
#define THREADS 8   // maximum number of threads

#define ALLOCATE(size, type) (type*)malloc(size * sizeof(type))

int min(int a, int b) {
    return a < b ? a : b;
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
bool relax(double *in, double *out, bool *stable) {
    int i, id, threads, steps, start, end;
    memset(stable, true, THREADS * sizeof(bool));

    #pragma omp parallel private(i, id, threads, steps)
    {
        id = omp_get_thread_num();
        threads = omp_get_num_threads();

        steps = ceil((double)N / threads);
        start = id * steps + 1;
        end = min(start + steps, N - 1);

        for (i = start; i < end; i++) {
            out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];

            if (stable[id] && fabs(in[i] - out[i]) > EPS) {
                stable[id] = false;
            }
        }
    }

    for (i = 0; i < THREADS; i++) {
        if (!stable[i]) {
            return false;
        }
    }

    return true;
}

int main() {
    double *old, *new, *tmp;
    bool *stable;

    old = ALLOCATE(N, double);
    new = ALLOCATE(N, double);
    stable = ALLOCATE(THREADS, bool);

    init(old, N);
    init(new, N);

    int iterations = 1;
    double start = omp_get_wtime();
    omp_set_num_threads(THREADS);

    while (!relax(old, new, stable)) {
        tmp = old;
        old = new;
        new = tmp;

        iterations++;
    }

    double end = omp_get_wtime();
    printf("Iterations: %d\n", iterations);
    printf("Duration: %fs\n", end - start);

    return 0;
}
