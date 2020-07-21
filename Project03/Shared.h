#pragma once

#include <stdio.h>
#include <fstream>
#include <math.h>
#include <memory>

#define N 100
#define HEAT 400.0
#define EPS 0.05

#define STEPS 50
#define REPEATS 10

class Shared {
public:
    /// <summary>Allocate a flattened matrix and initialise the values.</summary>
    /// <param name="size">The total size of the matrix.</param>
    /// <param name="heatIndex">At what index to place the heat, -1 if no heat should be added.</param>
    /// <param name="heat">The heat value to place.</param>
    /// <returns>A new matrix with the given size and potentially a heat value.</returns>
    inline static double* CreateMatrix(size_t size, int heatIndex = -1, double heat = HEAT) {
        double* m = (double*)calloc(size, sizeof(double));
        if (heatIndex >= 0) {
            m[heatIndex] = heat;
        }
        return m;
    }

    /// <summary>Diffuses a point using neighbouring values.</summary>
    /// <param name="in">The original matrix.</param>
    /// <param name="out">The resulting matrix.</param>
    /// <param name="n">The width of the matrix.</param>
    /// <param name="i">The point to diffuse.</param>
    inline static void Diffuse(double* in, double* out, size_t n, size_t i) {
        out[i] = 0.25 * in[i]    // center
            + 0.250 * in[i - n]  // upper
            + 0.125 * in[i + n]  // lower
            + 0.175 * in[i - 1]  // left
            + 0.200 * in[i + 1]; // right
    }

    /// <summary>Opens a csv file and returns it.</summary>
    /// <param name="filename">The name of the file to open.</param>
    /// <returns>The opened file.</returns>
    inline static std::ofstream OpenFile(const std::string filename) {
        std::ofstream file;
        std::string path = "Evaluation/" + filename + ".csv";
        file.open(path, std::ios_base::app);

        if (!file.is_open()) {
            printf("Could not open file '%s.csv'.", filename);
            exit(1);
        }

        return file;
    }

    /// <summary>Write information about the state of the program.</summary>
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
