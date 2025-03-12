#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLEN 100

const char* greetings[] = { "Hello", "Hi", "Awaiting your command" };
char buff[MAXLEN];

int main(int argc, char** argv) {
	int grID, rank;
	srand(time(0));
	MPI_Init(&argc, &argv);
	grID = rand() % 3;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	sprintf_s(buff, "Node %i says %s", rank, greetings[grID]);
	MPI_Send(buff, strlen(buff), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	MPI_Finalize();
}