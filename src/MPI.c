#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include "relax.h"
#include <stdbool.h>
#include <math.h>

void init(double *out, int n) {
	memset(out, 0, n * sizeof(double));
	out[0] = HEAT;
}

bool relax(double *in, double *out, int n, int th, int my_rank) {
	int local_start, local_stop, local_n, source; 
	bool local_stable, stable;
	double result;
	local_stable = true;
	stable = true;
	local_n = n/th;	
	local_start = my_rank * local_n;
	local_stop = local_start + local_n;

	printf("my_rank, local_start, local_stop: %d, %d, %d \n", my_rank, local_start, local_stop);
	
	for (int i = local_start + 1; i < local_stop - 1; i++) {
		out[i] = 0.25 * in[i - 1] + 0.5 * in[i] + 0.25 * in[i + 1];
        	if (stable && fabs(in[i] - out[i]) > EPS) {
			printf("stable = false");
        		stable = false;
        	}
    	}
/*
	if (my_rank != 0){
		if (stable == false){
			result = 0;
			MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}
		else{
			result = 1;
			MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}
		
	}
	else {
		stable = local_stable;
		for (source = 1; source < th; source++){
			MPI_Recv(&result, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			stable = stable && local_stable;
		}
		
	}*/
    	return stable;
}

void run(int n, int my_rank, int th) {	
	double *old, *new, *tmp;
	int local_iterations = 1;
	clock_t local_clock_end;

	old = ALLOCATE(double, n);
	new = ALLOCATE(double, n);

	printf("old: %f \n", old[0]);
	int iterations = 1;
	clock_t start = clock();	
	while (!relax(old, new, n, th, my_rank)) {
		printf("iterations: %d \n", iterations);
		tmp = old;
        	old = new;
        	new = tmp;

        	iterations++;

	}
	clock_t end = clock();

//	if (my_rank != 0){
//		MPI_Send(&local_iterations, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
//	}
//	else{
//		iterations = local_iterations;
//		for (int source = 1; source < th; source++){
//			MPI_Recv(&local_iterations, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			iterations+=local_iterations;
//		}
//	}

	printf("%d,%f,%f,%d,%d,%f\n", n, HEAT, EPS, th, iterations, (double)(end - start) / CLOCKS_PER_SEC);

	free(old);
	free(new);
}


int main() {
	int t = 500000, my_rank, th;
	printf("size,heat,eps,threads,iterations,duration\n");
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &th);
	run(EVAL_START, my_rank, th);
	MPI_Finalize();
	return 0;
}
