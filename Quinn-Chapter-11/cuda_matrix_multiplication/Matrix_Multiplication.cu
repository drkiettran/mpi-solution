
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

// MatrixMul_CUDA.cu
#include <iostream>
#include <string>
#include <vector>
#include <cuda_runtime.h>
#include <stdio.h>

cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);

using namespace std;

#define TILE_WIDTH 16  // Tile size for shared memory

/*
* Print matrix on screen.
*/
void printMatrix(char* text, const float*& matrix, int rows, int cols) {
    cout << "printMatrix text: " << text << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i][j] << ", ";
        }
        cout << endl;
    }
    return;
}

__global__ void matrixMulShared(float* A, float* B, float* C, int M, int N, int K) {
    // Shared memory for A and B tiles
    __shared__ float s_A[TILE_WIDTH][TILE_WIDTH];
    __shared__ float s_B[TILE_WIDTH][TILE_WIDTH];

    // Row and Column index of C element
    int row = blockIdx.y * TILE_WIDTH + threadIdx.y;
    int col = blockIdx.x * TILE_WIDTH + threadIdx.x;

    float value = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (K + TILE_WIDTH - 1) / TILE_WIDTH; ++t) {
        if (row < M && t * TILE_WIDTH + threadIdx.x < K)
            s_A[threadIdx.y][threadIdx.x] = A[row * K + t * TILE_WIDTH + threadIdx.x];
        else
            s_A[threadIdx.y][threadIdx.x] = 0.0f;

        if (col < N && t * TILE_WIDTH + threadIdx.y < K)
            s_B[threadIdx.y][threadIdx.x] = B[(t * TILE_WIDTH + threadIdx.y) * N + col];
        else
            s_B[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        for (int k = 0; k < TILE_WIDTH; ++k)
            value += s_A[threadIdx.y][k] * s_B[k][threadIdx.x];

        __syncthreads();
    }

    if (row < M && col < N)
        C[row * N + col] = value;
}

void cpuMatrixMultiply(float* A, float* B, float* C, int M, int N, int K) {
    for (int row = 0; row < M; ++row)
        for (int col = 0; col < N; ++col) {
            float sum = 0;
            for (int k = 0; k < K; ++k)
                sum += A[row * K + k] * B[k * N + col];
            C[row * N + col] = sum;
        }
}

int main() {
    int M = 256, N = 256, K = 256;

    size_t size_A = M * K * sizeof(float);
    size_t size_B = K * N * sizeof(float);
    size_t size_C = M * N * sizeof(float);

    float* h_A = new float[M * K];
    float* h_B = new float[K * N];
    float* h_C = new float[M * N];
    float* h_C_ref = new float[M * N];

    // Initialize matrices
    for (int i = 0; i < M * K; ++i) h_A[i] = static_cast<float>(i % 100) / 100.0f;
    for (int i = 0; i < K * N; ++i) h_B[i] = static_cast<float>((i + 1) % 100) / 100.0f;
    printMatrix("Initialized ", h_A, M, K);
    // CPU reference result
    cpuMatrixMultiply(h_A, h_B, h_C_ref, M, N, K);

    float* d_A, * d_B, * d_C;
    cudaMalloc(&d_A, size_A);
    cudaMalloc(&d_B, size_B);
    cudaMalloc(&d_C, size_C);

    cudaMemcpy(d_A, h_A, size_A, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size_B, cudaMemcpyHostToDevice);

    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH);
    dim3 dimGrid((N + TILE_WIDTH - 1) / TILE_WIDTH, (M + TILE_WIDTH - 1) / TILE_WIDTH);

    matrixMulShared << <dimGrid, dimBlock >> > (d_A, d_B, d_C, M, N, K);
    cudaDeviceSynchronize();

    cudaMemcpy(h_C, d_C, size_C, cudaMemcpyDeviceToHost);

    // Compare results
    bool match = true;
    for (int i = 0; i < M * N; ++i) {
        if (fabs(h_C[i] - h_C_ref[i]) > 1e-3) {
            std::cout << "Mismatch at " << i << ": GPU=" << h_C[i] << ", CPU=" << h_C_ref[i] << "\n";
            match = false;
            break;
        }
    }

    std::cout << (match ? "✅ Matrix multiplication passed.\n" : "❌ Mismatch in results.\n");

    // Cleanup
    delete[] h_A;
    delete[] h_B;
    delete[] h_C;
    delete[] h_C_ref;
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    return 0;
}
