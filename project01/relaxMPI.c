#include <mpi.h>
#include <time.h>
#include "relax.h"

void init(double *out, int n) {
    memset(out, 0, n * sizeof(double));
    out[0] = HEAT;
}

bool relax(double *in, double *out, int n, int th, int my_rank) {
    int local_start, local_stop, local_n;
    bool stable  = true;
    local_n = n/th;
    local_start = my_rank * local_n;
    local_stop = local_start + local_n;

    for (int i = local_start + 1; i < local_stop - 1; i++) {
        out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];

        if (stable && fabs(in[i] - out[i]) > EPS) {
            stable = false;
        }
    }

    return stable;
}

void run(int n, int my_rank, int th) {
    double *old, *new, *tmp;
    int local_iterations = 1;

    old = ALLOCATE(double, n);
    new = ALLOCATE(double, n);

    init(old, n);
    init(new, n);

    int iterations = 0;
    clock_t start = clock();

    while (!relax(old, new, n, th, my_rank)) {
        tmp = old;
        old = new;
        new = tmp;

        local_iterations++;
    }

    clock_t end = clock();

    if (my_rank != 0){
        MPI_Send(&local_iterations, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else{
        iterations = local_iterations;
        for (int source = 1; source < th; source++){
            MPI_Recv(&local_iterations, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            iterations += local_iterations;
        }
    }

    if (my_rank == 0){
        printf("%d,%f,%f,%d,%d,%f\n", n, HEAT, EPS, th,
                iterations, (double)(end - start) / CLOCKS_PER_SEC);
    }

    free(old);
    free(new);
}


int main() {
    int my_rank, th;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &th);

    if (my_rank == 0){
        printf("size,heat,eps,threads,iterations,duration\n");
    }

    for (int i = 1; i <= EVAL_STEPS; i++) {
        for (int r = 0; r < EVAL_REPEATS; r++){
            run(EVAL_START * i, my_rank, th);
        }
    }

    MPI_Finalize();
    return 0;
}
