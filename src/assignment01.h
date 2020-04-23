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

/**
=== Task 1 ===
Estimated 3.141593 in 0.890s with 100000000 steps and  1 threads
Estimated 3.141593 in 0.813s with 100000000 steps and  2 threads
Estimated 3.141593 in 0.750s with 100000000 steps and  3 threads
Estimated 3.141593 in 0.500s with 100000000 steps and  4 threads
Estimated 3.141593 in 0.468s with 100000000 steps and  5 threads
Estimated 3.141593 in 0.532s with 100000000 steps and  6 threads
Estimated 3.141593 in 0.500s with 100000000 steps and  7 threads
Estimated 3.141593 in 0.578s with 100000000 steps and  8 threads
Estimated 3.141593 in 0.547s with 100000000 steps and  9 threads
Estimated 3.141593 in 0.593s with 100000000 steps and 10 threads
Estimated 3.141593 in 0.563s with 100000000 steps and 11 threads
Estimated 3.141593 in 0.531s with 100000000 steps and 12 threads
Estimated 3.141593 in 0.484s with 100000000 steps and 13 threads
Estimated 3.141593 in 0.516s with 100000000 steps and 14 threads
Estimated 3.141593 in 0.422s with 100000000 steps and 15 threads
Estimated 3.141593 in 0.469s with 100000000 steps and 16 threads

===Task 2 ===
Estimated 3.141593 in 0.922s with 100000000 steps and  1 threads
Estimated 3.141593 in 0.609s with 100000000 steps and  2 threads
Estimated 3.141593 in 0.453s with 100000000 steps and  3 threads
Estimated 3.141593 in 0.344s with 100000000 steps and  4 threads
Estimated 3.141593 in 0.344s with 100000000 steps and  5 threads
Estimated 3.141593 in 0.312s with 100000000 steps and  6 threads
Estimated 3.141593 in 0.250s with 100000000 steps and  7 threads
Estimated 3.141593 in 0.250s with 100000000 steps and  8 threads
Estimated 3.141593 in 0.219s with 100000000 steps and  9 threads
Estimated 3.141593 in 0.250s with 100000000 steps and 10 threads
Estimated 3.141593 in 0.250s with 100000000 steps and 11 threads
Estimated 3.141593 in 0.203s with 100000000 steps and 12 threads
Estimated 3.141593 in 0.203s with 100000000 steps and 13 threads
Estimated 3.141593 in 0.187s with 100000000 steps and 14 threads
Estimated 3.141593 in 0.235s with 100000000 steps and 15 threads
Estimated 3.141593 in 0.218s with 100000000 steps and 16 threads

===Task 3 ===
Estimated 3.141593 in 0.594s with 100000000 steps and  1 threads
Estimated 3.141593 in 0.375s with 100000000 steps and  2 threads
Estimated 3.141593 in 0.328s with 100000000 steps and  3 threads
Estimated 3.141593 in 0.266s with 100000000 steps and  4 threads
Estimated 3.141593 in 0.203s with 100000000 steps and  5 threads
Estimated 3.141593 in 0.203s with 100000000 steps and  6 threads
Estimated 3.141593 in 0.188s with 100000000 steps and  7 threads
Estimated 3.141593 in 0.171s with 100000000 steps and  8 threads
Estimated 3.141593 in 0.188s with 100000000 steps and  9 threads
Estimated 3.141593 in 0.156s with 100000000 steps and 10 threads
Estimated 3.141593 in 0.156s with 100000000 steps and 11 threads
Estimated 3.141593 in 0.157s with 100000000 steps and 12 threads
Estimated 3.141593 in 0.157s with 100000000 steps and 13 threads
Estimated 3.141593 in 0.140s with 100000000 steps and 14 threads
Estimated 3.141593 in 0.141s with 100000000 steps and 15 threads
Estimated 3.141593 in 0.172s with 100000000 steps and 16 threads
**/
