#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <tclap/CmdLine.h>

double f(int i, int n) {
	double xi = (1.0 / (double)n) * ((double)i + .5);
	return 4.0 / (1.0 + xi * xi);
}

static int get_n(int argc, char** argv, int& n) {
	TCLAP::CmdLine cmd("Simpson's Rule", ' ', "1.0");
	TCLAP::ValueArg<int> nArg("n", "number", "value of n", true, 50, "int");
	cmd.add(nArg);
	cmd.parse(argc, argv);
	n = nArg.getValue();
	return 0;
}

/*
* What if n/2 is not divisible by p?
* What to do with the left over?
*/
int main(int argc, char* argv[]) {
	double global_area = 0;	/* Total number of solutions */
	int i, n, m;
	int id;					/* Process rank */
	int p;					/* Number of processes*/
	double area;			/* Solutions found by this proc */
	double elapsed_time;

	get_n(argc, argv, n);
	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (id == 0) {
		std::cout << "there are " << p << " processors...\n";
	}
	area = 0;
	for (i = id; i <= n; i += p) {
		area += f(i, n);
	}
	MPI_Reduce(&area, &global_area, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	elapsed_time += MPI_Wtime();

	printf("Process %d is done\n", id);

	fflush(stdout);
	MPI_Finalize();

	if (id == 0) {
		global_area *= (1.0 / (double)n);
		printf("Approximation of pi: %13.11f\n", global_area);
		printf("runtime: %f\n", elapsed_time);
	}
	return 0;
}