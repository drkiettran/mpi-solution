#include <mpi.h>
#include <iostream>
#include <cmath>
#include <vector>

void sieveOfEratosthenes(int n, int rank, int size) {
    // Determine the range for each process
    int low = (n / size) * rank + 1;
    int high = (rank == size - 1) ? n : (n / size) * (rank + 1);

    if (rank == 0) low = 2;  // Process 0 starts from 2

    // Create a boolean array "prime[low..high]" and initialize all entries as true
    std::vector<bool> isPrime(high - low + 1, true);

    int limit = std::sqrt(n);  // Limit for primes to be marked

    // Broadcast the primes from the small range handled by process 0
    if (rank == 0) {
        for (int p = 2; p <= limit; ++p) {
            if (isPrime[p - low]) {
                for (int i = p * p; i <= n; i += p) {
                    int idx = i - low;
                    if (idx >= 0 && idx < isPrime.size())
                        isPrime[idx] = false;
                }
            }
        }
    }

    // Distribute work to each process to mark the non-prime numbers
    for (int p = 2; p <= limit; ++p) {
        // Broadcast the prime number p to all processes
        MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Each process marks multiples of p within its range
        int start = std::max(p * p, low + (p - low % p) % p);  // Start at the first multiple of p in range
        for (int i = start; i <= high; i += p) {
            isPrime[i - low] = false;
        }
    }

    // Gather results at process 0 and print the primes
    if (rank == 0) {
        std::cout << "Primes up to " << n << ": ";
        for (int i = 0; i <= high - low; ++i) {
            if (isPrime[i]) std::cout << low + i << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    int n = 100;  // Find primes up to n

    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Start the Sieve of Eratosthenes
    sieveOfEratosthenes(n, rank, size);

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
