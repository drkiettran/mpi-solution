// barlas_5_5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>
#include <string.h>

#define MESSTAG 0

int MSB(int i) {
	int pos = 0; 
	while (i != 0) {
		i >>= 1;
		pos++;
	}
	return pos - 1;
}

/*******************************************/
int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int rank, num, i;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	MPI_Status status;
	if (rank == 0) {
		int destID = 1;
		double data;
		scanf("%lf", &data);
		while (destID < num) {
			MPI_Send(&data, 1, MPI_DOUBLE, destID, MESSTAG, MPI_COMM_WORLD);
		}
	}
	else {
		int msbPos = MSB(rank);
		int srcID = rank ^ (1 << msbPos);

	}
}