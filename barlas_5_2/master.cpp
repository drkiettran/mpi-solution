// barlas_5_2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>
#include <string.h>

#define MAXLEN 100

char buff[MAXLEN];
using namespace std;

int main(int argc, char** argv)
{
    MPI_Status st;
    int procNum;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    while (--procNum) {
        MPI_Recv(buff, MAXLEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
        int aux;
        MPI_Get_count(&st, MPI_CHAR, &aux);
        buff[aux] = 0;
        cout << buff << endl;
    }

    MPI_Finalize();
}

/*
* Build the project. output is stored under x64\debug folder under the solution folder.
*
* To run:
*   mpiexec -n 1 barlas_5_2_master.exe : -n 3 barlas_5_2_worker.exe
*
* Output:
*   Node 3 says Hi
*   Node 2 says Hi
*   Node 1 says Hi
*
* n defines the number of processors.
*
* To debug:
*  use the usual debugging techniques with visual studio.
*
*/