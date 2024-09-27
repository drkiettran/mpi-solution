#include <iostream>
#include <cmath>
#include <cuda_runtime.h>

// ChatGPT generated this code.

// Constants
__device__ const float G = 6.67430e-11f;  // Gravitational constant
const int N = 1000;            // Number of bodies
__device__ const float dt = 0.01f;        // Time step
const int num_timesteps = 100; // Number of simulation steps

// Structure to represent 3D vectors (position, velocity)
struct vec3 {
    float x, y, z;

    __host__ __device__
        vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    __host__ __device__
        vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // Vector addition
    __host__ __device__
        vec3 operator+(const vec3& b) const {
        return vec3(x + b.x, y + b.y, z + b.z);
    }

    // Scalar multiplication
    __host__ __device__
        vec3 operator*(float scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    // Vector subtraction
    __host__ __device__
        vec3 operator-(const vec3& b) const {
        return vec3(x - b.x, y - b.y, z - b.z);
    }

    // Vector magnitude
    __host__ __device__
        float magnitude() const {
        return sqrtf(x * x + y * y + z * z);
    }
};

// CUDA kernel to compute forces and update velocities and positions
__global__
void nbody_kernel(vec3* positions, vec3* velocities, float* masses, int num_bodies, float dt) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i >= num_bodies) return;

    vec3 pos_i = positions[i];
    vec3 force(0.0f, 0.0f, 0.0f);

    // Calculate net force on body i due to all other bodies
    for (int j = 0; j < num_bodies; ++j) {
        if (i != j) {
            vec3 pos_j = positions[j];
            vec3 diff = pos_j - pos_i;
            float dist = diff.magnitude() + 1e-10f; // Avoid division by zero
            float F = (G * masses[i] * masses[j]) / (dist * dist * dist);
            force = force + diff * F;
        }
    }

    // Update velocity
    vec3 vel_i = velocities[i];
    vec3 accel = force * (1.0f / masses[i]);
    vel_i = vel_i + accel * dt;

    // Update position
    vec3 pos_new = pos_i + vel_i * dt;

    // Write updated position and velocity back
    velocities[i] = vel_i;
    positions[i] = pos_new;
}

int main() {
    // Create random positions, velocities, and masses
    vec3* h_positions = new vec3[N];
    vec3* h_velocities = new vec3[N];
    float* h_masses = new float[N];

    // Initialize random positions, velocities, and masses
    for (int i = 0; i < N; ++i) {
        h_positions[i] = vec3(rand() % 100, rand() % 100, rand() % 100);
        h_velocities[i] = vec3(rand() % 10, rand() % 10, rand() % 10);
        h_masses[i] = rand() % 100 + 1; // Avoid zero mass
    }

    // Allocate device memory
    vec3* d_positions;
    vec3* d_velocities;
    float* d_masses;
    cudaMalloc(&d_positions, N * sizeof(vec3));
    cudaMalloc(&d_velocities, N * sizeof(vec3));
    cudaMalloc(&d_masses, N * sizeof(float));

    // Copy data to device
    cudaMemcpy(d_positions, h_positions, N * sizeof(vec3), cudaMemcpyHostToDevice);
    cudaMemcpy(d_velocities, h_velocities, N * sizeof(vec3), cudaMemcpyHostToDevice);
    cudaMemcpy(d_masses, h_masses, N * sizeof(float), cudaMemcpyHostToDevice);

    // Define block and grid sizes
    int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize;

    // Run the simulation
    for (int t = 0; t < num_timesteps; ++t) {
        // Launch kernel
        nbody_kernel << <numBlocks, blockSize >> > (d_positions, d_velocities, d_masses, N, dt);
        cudaDeviceSynchronize();
    }

    // Copy data back to host
    cudaMemcpy(h_positions, d_positions, N * sizeof(vec3), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_velocities, d_velocities, N * sizeof(vec3), cudaMemcpyDeviceToHost);

    // Print the final positions of a few bodies
    std::cout << "Final positions of some bodies:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "Body " << i << ": (" << h_positions[i].x << ", " << h_positions[i].y << ", " << h_positions[i].z << ")" << std::endl;
    }

    // Free memory
    delete[] h_positions;
    delete[] h_velocities;
    delete[] h_masses;
    cudaFree(d_positions);
    cudaFree(d_velocities);
    cudaFree(d_masses);

    return 0;
}
