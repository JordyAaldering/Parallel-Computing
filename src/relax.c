#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define N 10000000  // length of the vectors
#define EPS 0.1     // convergence criterium
#define HEAT 100.0  // heat value on the boundary

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
    out[0] = HEAT;
    for (int i = 1; i < n; i++) {
        out[i] = 0;
    }
}

void swap(double *a, double *b) {
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * print the values of a given vector "out" of length "n"
 */
void print(double *out, int n) {
    printf("<");
    for (int i = 0; i < n; i++) {
        printf(" %f", out[i]);
    }
    printf(">\n");
}

/**
 * individual step of the 3-point stencil
 * computes values in vector "out" from those in vector "in"
 * assuming both are of length "n"
 */
void relax(double *in, double *out, int n) {
    for (int i = 1; i < n - 1; i++) {
        out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];
    }
}

/**
 * checks the convergence criterion:
 * true, iff for all indices i, we have |out[i] - in[i]| <= eps
 */
bool isStable(double *old, double *new, int n, double eps) {
    bool res = true;
    for (int i = 1; i < n - 1; i++) {
        res &= fabs(old[i] - new[i]) <= eps;
    }

    return res;
}

int main() {
    int iterations = 0;
    double *a = allocVector(N);
    double *b = allocVector(N);

    init(a, N);
    init(b, N);

    printf("size   : %d M (%d MB)\n", N / 1000000, (int) (N * sizeof(double) / (1024 * 1024)));
    printf("heat   : %f\n", HEAT);
    printf("epsilon: %f\n", EPS);

    do {
        swap(a, b);
        relax(a, b, N);
        // print(b, N);
        iterations++;
    } while (!isStable(a, b, N, EPS));

    printf("Number of iterations: %d\n", iterations);
    return 0;
}
