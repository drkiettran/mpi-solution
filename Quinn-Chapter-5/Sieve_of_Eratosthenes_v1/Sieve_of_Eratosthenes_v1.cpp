
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <mpi.h>

#define N 10
using namespace std;

void print(std::string text, int rank, int size, int ss, int start, int end, vector<bool> lp) {
    char cbuf[1024];
    string sbuf;
    snprintf(cbuf, sizeof(cbuf), "\n==> rank: %d, text: %s size: %d\n", rank, text.c_str(), size);
    sbuf.append(cbuf);
    snprintf(cbuf, sizeof(cbuf), "ss: %d, start: %d, end: %d\n", ss, start, end);
    sbuf.append(cbuf);

    cout << sbuf << end;
}


void sieveSegment(int rank, int size, std::vector<bool>& local_primes, int start, int end) {
    print("sievseg starts ...", rank, size, 0, start, end, local_primes);
    for (int p = 2; p * p <= N; ++p) {
        if (rank == 0) { // Master process finds next prime
            if (!local_primes[p]) continue;
        }
        // Broadcast prime to all processes
        MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
        //print("after bcast ...", rank, size, p, start, end, local_primes);

        // Each process marks multiples of p in its range
        for (int i = std::max(p * p, (start + p - 1) / p * p); i <= end; i += p) {

            local_primes[i - start] = false;
        }
    }
    print("sievseg ends ...", rank, size, 0, start, end, local_primes);
}


int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering.
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    cout << "Starting ...\n\n";

    int segment_size = (N - 1) / size + 1; // Divide the range into roughly equal parts
    int start = rank * segment_size + 2;
    int end = std::min(N, start + segment_size - 1);

    std::vector<bool> local_primes(segment_size, true);

    // Perform parallel Sieve of Eratosthenes
    print("before call", rank, size, segment_size, start, end, local_primes);
    sieveSegment(rank, size, local_primes, start, end);
    print("after call", rank, size, segment_size, start, end, local_primes);

    // Gather results in the root process
    std::vector<bool> global_primes;
    if (rank == 0) global_primes.resize(N - 1, true);
    //MPI_Gather(local_primes.data(), segment_size, MPI_C_BOOL,
    //           global_primes.data(), segment_size, MPI_C_BOOL, 0, MPI_COMM_WORLD);

//    if (rank == 0) {
        std::cout << "Prime numbers up to " << N << ": ";
        for (int i = 2; i <= local_primes.size(); ++i) {
            if (local_primes[i - 2]) std::cout << i << " ";
        }
        std::cout << std::endl;
//    }

    MPI_Finalize();
    return 0;
}
