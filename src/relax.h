#ifndef PARALLEL_COMPUTING_RELAX_H
#define PARALLEL_COMPUTING_RELAX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define EVAL_START 1000000
#define EVAL_STEPS 10
#define EVAL_REPEATS 10
#define MAX_THREADS 8

#define HEAT 100.0
#define EPS 0.1

#define ALLOCATE(type, size) (type*)malloc(size * sizeof(type))

int min(int a, int b) {
    return a < b ? a : b;
}

#endif //PARALLEL_COMPUTING_RELAX_H
