#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>


using namespace std;

mutex mtx; // Mutex to synchronize access to the prime array

// Function to mark multiples of a prime number in a given range
void markMultiples(vector<bool>& prime, int p, int n) {
    for (int i = p * p; i <= n; i += p) {
        // Lock the critical section while updating shared data
        lock_guard<mutex> lock(mtx);
        prime[i] = false;
    }
}

// Function to parallelize the sieve using threads
void sieveOfEratosthenes(int n, int numThreads) {
    // Create a boolean array "prime[0..n]" and initialize all entries as true.
    vector<bool> prime(n + 1, true);
    prime[0] = prime[1] = false; // 0 and 1 are not primes

    int sqrt_n = static_cast<int>(sqrt(n));

    // Create threads for marking the multiples of primes
    vector<thread> threads;

    // Loop over each number up to sqrt(n)
    for (int p = 2; p <= sqrt_n; p++) {
        if (prime[p]) {
            // Launch multiple threads to mark the multiples of p
            for (int t = 0; t < numThreads; t++) {
                threads.push_back(thread(markMultiples, ref(prime), p, n));
            }

            // Wait for all threads to finish
            for (auto& th : threads) {
                th.join();
            }

            // Clear the thread vector for the next prime number
            threads.clear();
        }
    }

    // Print all prime numbers
    cout << "Prime numbers up to " << n << " are: ";
    for (int i = 2; i <= n; i++) {
        if (prime[i]) {
            cout << i << " ";
        }
    }
    cout << endl;
}

int main() {
    int n;
    int numThreads;

    cout << "Enter the limit: ";
    cin >> n;

    cout << "Enter the number of threads: ";
    cin >> numThreads;

    sieveOfEratosthenes(n, numThreads);

    return 0;
}
