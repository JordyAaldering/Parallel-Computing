#pragma once

#include <stdio.h>
#include <fstream>
#include <math.h>
#include <memory>

class Shared {

#define N 1000
#define HEAT 400.0
#define EPS 0.05

public:
    /// <summary> Allocate a flattened matrix of 'size' elements.
    /// Initialise the values of the matrix with 0s and 'heat'. </summary>
    inline static double* CreateMatrix(size_t size, int heatIndex = -1, double heat = HEAT) {
        double* m = (double*)calloc(size, sizeof(double));
        if (heatIndex >= 0) {
            m[heatIndex] = heat;
        }
        return m;
    }

    inline static void Diffuse(double* in, double* out, size_t n, size_t i) {
        out[i] = 0.25 * in[i]    // center
            + 0.250 * in[i - n]  // upper
            + 0.125 * in[i + n]  // lower
            + 0.175 * in[i - 1]  // left
            + 0.200 * in[i + 1]; // right
    }

    /// <summary> Write information about the program. </summary>
    inline static void WriteInfo(std::ofstream& file, int n, int iterations, int ms) {
        file << n << "," << (int)(n * n * sizeof(double) / (1024 * 1024))
            << "," << iterations << "," << ms << std::endl;
    }

};
