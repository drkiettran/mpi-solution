#include <iostream>
#include <cuda_runtime.h>

using namespace std;

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
