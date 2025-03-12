// barlas_5_1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>
#include <string.h>

#define MESSTAG 0
#define MAXLEN 100

using namespace std;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int rank, num, i;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num);

    if (rank == 0) {
        char mess[] = "Hello, world";
        int len = strlen(mess) + 1;
        for (i = 1; i < num; i++) {
            MPI_Send(mess, len, MPI_CHAR, i, MESSTAG, MPI_COMM_WORLD);
        }
    }
    else {
        char mess[MAXLEN];
        MPI_Status status;
        MPI_Recv(mess, MAXLEN, MPI_CHAR, 0, MESSTAG, MPI_COMM_WORLD, &status);
        cout << rank << " received " << mess << endl;
    }
    MPI_Finalize();
    return 0;
}

/*
* Build the project. output is stored under x64\debug folder under the solution folder.
* 
* To run: 
*   mpiexec -n 2 barlas_5_1.exe
* 
* Output:
*   1 received Hello, world
* 
* n defines the number of processors.
* 
* To debug:
*  use the usual debugging techniques with visual studio.
* 
*/