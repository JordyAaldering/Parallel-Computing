#pragma once

#include <stdio.h>
#include <fstream>
#include <time.h>
#include <memory>

class Util {
public:
    /// <summary>
    /// Allocate a flattened matrix of 'n by n' elements.
    /// Initialise the values of the matrix with 0s.
    /// </summary>
    inline static double* CreateMatrix(size_t n, double heat) {
        double* m = (double*)calloc(n * n, sizeof(double));
        m[n / 2] = heat;
        return m;
    }

    /// <summary> Print information about the program. </summary>
    inline static void PrintInfo(int n, double heat, double eps, int iterations, clock_t start, clock_t end) {
        printf("N         : %d\n", n);
        printf("Size      : %dMB\n", (int)(n * n * sizeof(double) / (1024 * 1024)));
        printf("Heat      : %f\n", heat);
        printf("Epsilon   : %f\n", eps);
        printf("Iterations: %d\n", iterations);
        printf("Time      : %dms\n", (int)((end - start) / (CLOCKS_PER_SEC / 1000)));
    }

    /// <summary> Write information about the program. </summary>
    inline static void WriteInfo(std::ofstream& file, int n, int iterations, clock_t start, clock_t end) {
        file << n << "," << iterations << "," << (int)((end - start) / (CLOCKS_PER_SEC / 1000)) << std::endl;
    }

    /// <summary> Print the values of the given matrix. </summary>
    inline static void PrintMatrix(double* out, size_t n) {
        for (size_t i = 0; i < n; i++) {
            printf("|");
            for (size_t j = 0; j < n; j++) {
                printf(" %6.2f", out[i * n + j]);
            }
            printf(" |\n");
        }
    }
};
