#include <time.h>
#include "relax.h"

void init(double *out, int n) {
    memset(out, 0, n * sizeof(double));
    out[0] = HEAT;
}

bool relax(double *in, double *out, int n) {
    bool stable = true;
    for (int i = 1; i < n - 1; i++) {
        out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];

        if (stable && fabs(in[i] - out[i]) > EPS) {
            stable = false;
        }
    }

    return stable;
}

void run(int n) {
    double *old, *new, *tmp;
    old = ALLOCATE(double, n);
    new = ALLOCATE(double, n);

    init(old, n);
    init(new, n);

    int iterations = 1;
    clock_t start = clock();

    while (!relax(old, new, n)) {
        tmp = old;
        old = new;
        new = tmp;

        iterations++;
    }

    clock_t end = clock();
    printf("%d, %f, %f, %d, %d, %f\n", n, HEAT, EPS, 1,
            iterations, (double)(end - start) / CLOCKS_PER_SEC);
}

int main() {
    printf("size, heat, eps, threads, iterations, duration\n");
    for (int i = 1; i <= EVAL_STEPS; i++) {
        for (int r = 0; r < EVAL_REPEATS; r++) {
            run(EVAL_START * i);
        }
    }

    return 0;
}
