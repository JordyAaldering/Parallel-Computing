#include <time.h>
#include "relax.h"

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
void relax(double *in, double *out, int n) {
    for (int i = 1; i < n - 1; i++) {
        out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];
    }
}

/**
 * checks the convergence criterion:
 * true, iff for all indices i, we have |out[i] - in[i]| <= eps
 */
bool isStable(double *old, double *new, int n) {
    bool res = true;
    for (int i = 1; i < n - 1; i++) {
        res &= fabs(old[i] - new[i]) <= EPS;
    }

    return res;
}

void run(int n) {
    double *old, *new, *tmp;
    old = ALLOCATE(double, n);
    new = ALLOCATE(double, n);

    init(old, n);
    init(new, n);

    int iterations = 0;
    clock_t start = clock();

    do {
        tmp = old;
        old = new;
        new = tmp;

        relax(old, new, n);
        iterations++;
    } while (!isStable(old, new, n));

    clock_t end = clock();
    printf("%d,%f,%f,%d,%d,%f\n", n, HEAT, EPS, 1,
            iterations, (double)(end - start) / CLOCKS_PER_SEC);

    free(old);
    free(new);
}

int main() {
    printf("n,heat,eps,threads,iterations,duration\n");
    for (int i = 1; i <= EVAL_STEPS; i++) {
        for (int r = 0; r < EVAL_REPEATS; r++) {
            run(EVAL_START * i);
        }
    }

    return 0;
}
