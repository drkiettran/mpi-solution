#include <iostream>
#include <cuda_runtime.h>
#include <limits>

const int INF = std::numeric_limits<int>::max();  // Infinity

__global__ void floydWarshallKernel(int* d_dist, int k, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;  // Row index
    int j = blockIdx.y * blockDim.y + threadIdx.y;  // Column index

    if (i < n && j < n) {
        int ij = i * n + j;
        int ik = i * n + k;
        int kj = k * n + j;

        // Check if the path i -> k -> j is shorter
        if (d_dist[ik] != INF && d_dist[kj] != INF && d_dist[ik] + d_dist[kj] < d_dist[ij]) {
            d_dist[ij] = d_dist[ik] + d_dist[kj];
        }
    }
}

void floydWarshallCUDA(int* dist, int n) {
    int size = n * n * sizeof(int);
    int* d_dist;

    // Allocate memory on the device
    cudaMalloc((void**)&d_dist, size);

    // Copy the distance matrix to the device
    cudaMemcpy(d_dist, dist, size, cudaMemcpyHostToDevice);

    // Define block and grid sizes
    dim3 threadsPerBlock(16, 16);  // 16x16 threads per block
    dim3 blocksPerGrid((n + threadsPerBlock.x - 1) / threadsPerBlock.x,
        (n + threadsPerBlock.y - 1) / threadsPerBlock.y);

    // Run the Floyd-Warshall algorithm
    for (int k = 0; k < n; ++k) {
        floydWarshallKernel << <blocksPerGrid, threadsPerBlock >> > (d_dist, k, n);
        cudaDeviceSynchronize();  // Ensure all threads finish before next iteration
    }

    // Copy the result back to the host
    cudaMemcpy(dist, d_dist, size, cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_dist);
}

void printSolution(const int* dist, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dist[i * n + j] == INF)
                std::cout << "INF ";
            else
                std::cout << dist[i * n + j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Example graph represented as an adjacency matrix
    int n = 4;
    int graph[16] = {
        0, 3, INF, 7,
        8, 0, 2, INF,
        5, INF, 0, 1,
        2, INF, INF, 0
    };

    // Run Floyd-Warshall algorithm on the GPU
    floydWarshallCUDA(graph, n);

    // Print the solution
    std::cout << "Shortest distances between every pair of vertices:\n";
    printSolution(graph, n);

    return 0;
}
