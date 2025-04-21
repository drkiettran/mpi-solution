#include <iostream>
#include <cuda_runtime.h>

using namespace std;

/*
* Algorithm: Search for all prime numbers within a range of integer values (i.e., 1-100)
* A prime number is a number that can be divided by 1 and by itself only. Prime number can only
* be an `odd` number.
*
* 1. Create a list of natural number 2,3,4, ... (i.e., 100). None of which is marked.
* 2. Set k=2 the first unmarked number on the list.
* 3. Repeat
*    - Mark all multiples of k between k^2 and n.
*    - Find the smallest number greater than k that is unmarked. Set to this new value.
*    - Until k^2 > n .
*
* The unmarked numbers are primes.
*
* example: prime numbers between 0 and 10.
* 1. [F, F, F, F, F, F, F, F, F]
* 2. k = 2
* 3. Repeat:
*    - k^2 = 4. Multiple of 2 is 4, 6, 8, 10 --> [F, F, T, F, T, F, T, F, T];
*    - Smallest greater than k=2 that is unmarked --> k=3
*    - k^2=3^2=9 > 10? NO
*    - k^2 = 9. Multiple of 3 is 6, 9 --> [F, F, T, F, T, F, T, T, T];
*    - Smallest greater than k=3 that is unmarked --> k=5
*    - k^2=5^2=25 > 10? YES.
* 4. WE ARE DONE. [T, T, T, F, T, F, T, T, T] --> 2, 3, 5, 7
*
*/

__global__ void sieveKernel(bool* d_prime, int n, int sqrt_n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x + 2;

    if (idx <= sqrt_n && d_prime[idx]) {
        // Mark all multiples of idx as false
        for (int i = idx * idx; i <= n; i += idx) {
            d_prime[i] = false;
        }
    }
}

void sieveOfEratosthenes(int n) {
    // Allocate memory on host (CPU)
    // vector<bool> prime(n + 1, true); 

    // Allocate memory on device (GPU)
    bool *d_prime, *h_prime;
    h_prime = (bool*) malloc(sizeof(bool) * (n + 1));
    for (int i = 0; i < (n + 1); i++) {
        h_prime[i] = true;
    }
    cudaMalloc((void**)&d_prime, (n + 1) * sizeof(bool));

    // Copy the data from host to device
    cudaMemcpy(d_prime, h_prime, (n + 1) * sizeof(bool), cudaMemcpyHostToDevice);

    // Calculate the block size and grid size
    int sqrt_n = sqrt(n);
    int blockSize = 256;
    int gridSize = (sqrt_n + blockSize - 1) / blockSize;

    // Launch the kernel
    sieveKernel << <gridSize, blockSize >> > (d_prime, n, sqrt_n);

    // Copy the result back to the host
    cudaMemcpy(h_prime, d_prime, (n + 1) * sizeof(bool), cudaMemcpyDeviceToHost);

    // Free the device memory
    cudaFree(d_prime);

    // Print the prime numbers
    cout << "Prime numbers up to " << n << " are: ";
    for (int i = 2; i <= n; i++) {
        if (h_prime[i])
            cout << i << " ";
    }
    cout << endl;
}

int main() {
    int n;
    cout << "Enter the limit: ";
    cin >> n;

    sieveOfEratosthenes(n);

    return 0;
}
