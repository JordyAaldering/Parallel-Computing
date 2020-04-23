#ifndef PARALLEL_COMPUTING_ASSIGNMENT01_H
#define PARALLEL_COMPUTING_ASSIGNMENT01_H

#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

double estimatePi1(int steps, int maxThreads) {
    int i, id, threads;
    double step = 1.0 / (double)steps;
    double x, sum[maxThreads], pi = 0;

    memset(sum, 0, maxThreads * sizeof(double));

    omp_set_num_threads(maxThreads);
    #pragma omp parallel private(i, id, threads, x)
    {
        id = omp_get_thread_num();
        threads = omp_get_num_threads();
        if (id == 0) {
            maxThreads = threads;
        }

        for (i = id; i < steps; i = i + threads) {
            x = (i + 0.5) * step;
            sum[id] += 4.0 / (1.0 + x * x);
        }
    }

    for (i = 0; i < maxThreads; i++) {
        pi += sum[i] * step;
    }

    return pi;
}

double estimatePi2(int steps, int maxThreads) {
    int i, id, threads;
    double step = 1.0 / (double)steps;
    double x, sum[maxThreads][64], pi = 0;

    for (i = 0; i < maxThreads; i++) {
        sum[i][0] = 0.0;
    }

    omp_set_num_threads(maxThreads);
    #pragma omp parallel private(i, id, threads, x)
    {
        id = omp_get_thread_num();
        threads = omp_get_num_threads();
        if (id == 0) {
            maxThreads = threads;
        }

        for (i = id; i < steps; i = i + threads) {
            x = (i + 0.5) * step;
            sum[id][0] += 4.0 / (1.0 + x * x);
        }
    }

    for (i = 0; i < maxThreads; i++) {
        pi += sum[i][0] * step;
    }

    return pi;
}

double estimatePi3(int steps, int threads) {
    int i;
    double step = 1.0 / (double)steps;
    double x, sum = 0.0;

    omp_set_num_threads(threads);
    #pragma omp parallel private(i, x)
    {
        #pragma omp for reduction(+:sum) schedule(static)
        for (i = 0; i < steps; i = i + 1) {
            x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }
    }

    return sum * step;
}

#define MEASURE_PERFORMANCE(function, steps, threads) do { \
        clock_t start = clock(); \
        double pi = function(steps, threads); \
        clock_t end = clock(); \
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC; \
        printf("Estimated %.6f in %.3fs with %d steps and %2d threads\n", pi, elapsed, steps, threads); \
    } while (false);

void runAssignment01() {
    int steps = 100000000;
    int maxThreads = 16;

    printf("\n=== Task 1 ===\n");
    for (int i = 1; i <= maxThreads; i++) {
        MEASURE_PERFORMANCE(estimatePi1, steps, i)
    }

    printf("\n===Task 2 ===\n");
    for (int i = 1; i <= maxThreads; i++) {
        MEASURE_PERFORMANCE(estimatePi2, steps, i)
    }

    printf("\n===Task 3 ===\n");
    for (int i = 1; i <= maxThreads; i++) {
        MEASURE_PERFORMANCE(estimatePi3, steps, i)
    }
}

#endif //PARALLEL_COMPUTING_ASSIGNMENT01_H
