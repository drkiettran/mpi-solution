#include <iostream>
#include <vector>
#include <omp.h>  // Include OpenMP header

using namespace std;

void sieveOfEratosthenes(int n) {
    // Create a boolean array "prime[0..n]" and initialize all entries as true.
    vector<bool> prime(n + 1, true);

    int sqrt_n = static_cast<int>(sqrt(n));

#pragma omp parallel for
    for (int p = 2; p <= sqrt_n; p++) {
        if (prime[p]) {
            // Mark all multiples of p as false, starting from p*p.
#pragma omp parallel for
            for (int i = p * p; i <= n; i += p) {
                prime[i] = false;
            }
        }
    }

    // Print all prime numbers
    cout << "Prime numbers up to " << n << " are: ";
    for (int p = 2; p <= n; p++) {
        if (prime[p])
            cout << p << " ";
    }
    cout << endl;
}

int main() {
    int n;
    cout << "Enter the limit: ";
    cin >> n;

    // Call the sieve function
    sieveOfEratosthenes(n);

    return 0;
}
