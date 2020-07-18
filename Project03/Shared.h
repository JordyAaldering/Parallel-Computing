#pragma once

#include <stdio.h>
#include <fstream>
#include <math.h>
#include <memory>

#define N 1000
#define HEAT 400.0
#define EPS 0.05

#define STEPS 50
#define REPEATS 5

class Shared {
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

    inline static std::ofstream OpenFile(const std::string filename) {
        std::ofstream file;
        std::string path = "Evaluation/" + filename + ".csv";
        file.open(path);

        if (!file.is_open()) {
            printf("Could not open file '%s.csv'.", filename);
            exit(1);
        }

        return file;
    }

    /// <summary> Write information about the program. </summary>
    inline static void WriteInfo(std::ofstream& file, int n, int iterations, int ms, int cores = -1) {
        if (cores > 0) {
            file << cores << ",";
        }

        file << n << "," 
             << (int)(n * n * sizeof(double) / (1024 * 1024)) << ","
             << iterations << ","
             << ms << std::endl;
    }
};
