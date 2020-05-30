#include <omp.h>
#include "relax.h"

void init(double *out, int n) {
    memset(out, 0, n * sizeof(double));
    out[0] = HEAT;
}

bool relax(double *in, double *out, bool *stable, int n, int maxThreads) {
    int i, id, threads, steps, start, end;
    memset(stable, true, maxThreads * sizeof(bool));

    #pragma omp parallel private(i, id, threads, steps)
    {
        id = omp_get_thread_num();
        threads = omp_get_num_threads();

        steps = ceil((double)n / threads);
        start = id * steps + 1;
        end = min(start + steps, n - 1);

        for (i = start; i < end; i++) {
            out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];

            if (stable[id] && fabs(in[i] - out[i]) > EPS) {
                stable[id] = false;
            }
        }
    }

    for (i = 0; i < maxThreads; i++) {
        if (!stable[i]) {
            return false;
        }
    }

    return true;
}

void run(int n, int threads) {
    double *old, *new, *tmp;
    bool *stable;

    old = ALLOCATE(double, n);
    new = ALLOCATE(double, n);
    stable = ALLOCATE(bool, threads);

    init(old, n);
    init(new, n);

    int iterations = 1;
    double start = omp_get_wtime();
    omp_set_num_threads(threads);

    while (!relax(old, new, stable, n, threads)) {
        tmp = old;
        old = new;
        new = tmp;

        iterations++;
    }

    double end = omp_get_wtime();
    printf("%d, %f, %f, %d, %d, %f\n", n, HEAT, EPS, threads, iterations, end - start);
}

int main() {
    printf("size, heat, eps, threads, iterations, duration\n");
    for (int i = 1; i <= EVAL_STEPS; i++) {
        for (int t = 1; t <= MAX_THREADS; t++) {
            for (int r = 0; r < EVAL_REPEATS; r++) {
                run(EVAL_START * i, t);
            }
        }
    }

    return 0;
}
