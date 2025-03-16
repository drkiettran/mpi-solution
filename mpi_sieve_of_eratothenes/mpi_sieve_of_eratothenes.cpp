#include <mpi.h>
#include <iostream>
#include <cmath>
#include <vector>

#define DEBUG

using namespace std;

typedef struct state {
    int rank;
    int size;
    int n;
    int low;
    int high;
    vector<bool> isPrime;
    vector<int> thePrimes;
} State;

/*
* Print flat matrix (one-dim) on screen.
*/
template <typename T>
string printFlatMatrix(int rank, string text, const vector<T> flatMatrix) {
    string sbuf;
    char buf[1024];
    sprintf_s(buf, "printFlatMatrix, rank: %d, text: %s\n", rank, text.c_str());
    sbuf += buf;
    if (flatMatrix.size() != 0) {
        for (size_t i = 0; i < flatMatrix.size(); i++) {
            sprintf_s(buf, "%d, ", flatMatrix[i]);
            sbuf += buf;
        }
    }
    else {
        sbuf += "** EMPTY **";
    }
    sbuf += "\n";
    // cout << sbuf << endl;
    return sbuf;
}

void printState(State state, string text) {
#ifdef DEBUG
    char cbuf[1024];
    string sbuf;
    sprintf_s(cbuf, "\n==> rank: %d, text: %s size: %d\n", state.rank, text.c_str(), state.size);
    sbuf.append(cbuf);
    sprintf_s(cbuf, "low: %d, high: %d\n", state.low, state.high);
    sbuf.append(cbuf);
    sbuf.append(printFlatMatrix(state.rank, "printState isPrimes", state.isPrime));
    sbuf.append(cbuf);
    sbuf.append(printFlatMatrix(state.rank, "printState thePrimes", state.thePrimes));
    sbuf.append(cbuf);
    cout << sbuf << endl;
#endif

}

void sieveOfEratosthenes(State& state) {
    // Determine the range for each process
    state.low = (state.n / state.size) * state.rank + 1;
    state.high = (state.rank == state.size - 1) ? state.n : (state.n / state.size) * (state.rank + 1);

    printState(state, "stage 1");
    if (state.rank == 0) {
        state.low = 2;  // Process 0 starts from 2
    }

    // Create a boolean array "prime[low..high]" and initialize all entries as true
    state.isPrime.resize(state.high - state.low + 1, true);

    int limit = std::sqrt(state.n);  // Limit for primes to be marked

    printState(state, "stage 2");
    // Broadcast the primes from the small range handled by process 0
    if (state.rank == 0) {
        for (int p = 2; p <= limit; ++p) {
            if (state.isPrime[p - state.low]) {
                for (int i = p * p; i <= state.n; i += p) {
                    int idx = i - state.low;
                    if (idx >= 0 && idx < state.isPrime.size())
                        state.isPrime[idx] = false;
                }
            }
        }
    }

    printState(state, "stage 3");
    // Distribute work to each process to mark the non-prime numbers
    for (int p = 2; p <= limit; ++p) {
        // Broadcast the prime number p to all processes
        MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Each process marks multiples of p within its range
        int start = max(p * p, state.low + (p - state.low % p) % p);  // Start at the first multiple of p in range
        for (int i = start; i <= state.high; i += p) {
            state.isPrime[i - state.low] = false;
        }
    }

    printState(state, "stage 4");
    // Gather results at process 0 and print the primes
    vector<int> primes;
    int segSize = state.n / state.size;
    primes.resize(segSize, 0);
 
    int pi = 0;
    for (int i = 0; i <= state.high - state.low; ++i) {
        // if (isPrime[i]) std::cout << low + i << " ";
        if (state.isPrime[i]) {
            primes[pi++] = state.low + i;
        }
    }
 
    int* pp = new int(primes.size()+1);
    copy(primes.begin(), primes.end(), pp);

    int* buffer = new int(state.n + 1);
    if (state.rank == 0)
    {
        for (int i = 0; i < state.n + 1; i++) {
            buffer[i] = 0;
        }
    }
    printState(state, "stage 5");
	MPI_Gather(pp, primes.size(), MPI_INT, buffer, primes.size(), MPI_INT, 0, MPI_COMM_WORLD);

    printState(state, "stage 6");
    if (state.rank == 0) {
        printState(state, "state 6.1");
        std::cout << "\n\nPrimes up to " << state.n << ": ";
        for (int i = 0; i < state.n + 1; i++) {
            char cbuf[100];
            sprintf_s(cbuf, "stage 6.1.%d", i);
            printState(state, cbuf);

            if (buffer[i] != 0) {
                state.thePrimes.push_back(buffer[i]);
                cout << buffer[i] << ", ";
            }

        }
        printState(state, "state 6.2");
        std::cout << std::endl;

    }
    printState(state, "stage 7");
}


int main(int argc, char* argv[]) {
    int n = 100;  // Find primes up to n

    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    n = atoi(argv[1]);
    State state;
    state.rank = rank;
    state.size = size;
    state.n = n;

    printState(state, "Before calls ...");
    // Start the Sieve of Eratosthenes
    sieveOfEratosthenes(state);
    printState(state, "After calls ...");

    // Finalize MPI
    MPI_Finalize();
    printState(state, "Exiting ...");
    return 0;
}



/**
 * @author RookieHPC
 * @brief Original source code at https://rookiehpc.org/mpi/docs/mpi_gather/index.html
 **/

/*
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

*/

 /**
  * @brief Illustrates how to use a gather.
  * @details This application is meant to be run with 4 MPI processes. Every MPI
  * process begins with a value, then MPI process 0 is picked to gather all these
  * values and print them. It can be visualised as follows:
  *
  * +-----------+ +-----------+ +-----------+ +-----------+
  * | Process 0 | | Process 1 | | Process 2 | | Process 3 |
  * +-+-------+-+ +-+-------+-+ +-+-------+-+ +-+-------+-+
  *   | Value |     | Value |     | Value |     | Value |
  *   |   0   |     |  100  |     |  200  |     |  300  |
  *   +-------+     +-------+     +-------+     +-------+
  *            \            |     |            /
  *             \           |     |           /
  *              \          |     |          /
  *               \         |     |         /
  *                \        |     |        /
  *                 \       |     |       /
  *                +-----+-----+-----+-----+
  *                |  0  | 100 | 200 | 300 |
  *                +-----+-----+-----+-----+
  *                |       Process 0       |
  *                +-----------------------+
  **/

/*
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Get number of processes and check that 4 processes are used
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 4)
    {
        printf("This application is meant to be run with 4 MPI processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Determine root's rank
    int root_rank = 0;

    // Get my rank
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Define my value
    int my_value = my_rank * 100;
    printf("Process %d, my value = %d.\n", my_rank, my_value);

    if (my_rank == root_rank)
    {
        int buffer[4];
        MPI_Gather(&my_value, 1, MPI_INT, buffer, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
        printf("Values collected on process %d: %d, %d, %d, %d.\n", my_rank, buffer[0], buffer[1], buffer[2], buffer[3]);
    }
    else
    {
        MPI_Gather(&my_value, 1, MPI_INT, NULL, 0, MPI_INT, root_rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}

*/