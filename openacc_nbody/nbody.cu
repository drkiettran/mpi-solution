﻿#include <iostream>
#include <cmath>
#include <openacc.h>

// Physical constants
const float G = 6.67430e-11f;  // Gravitational constant
const float dt = 0.01f;        // Time step
const int num_timesteps = 100; // Number of simulation steps
const int N = 1000;            // Number of bodies

// Structure to represent 3D vectors (position, velocity, force)
struct vec3 {
    float x, y, z;

    vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // Vector addition
    vec3 operator+(const vec3& b) const {
        return vec3(x + b.x, y + b.y, z + b.z);
    }

    // Vector subtraction
    vec3 operator-(const vec3& b) const {
        return vec3(x - b.x, y - b.y, z - b.z);
    }

    // Scalar multiplication
    vec3 operator*(float scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    // Vector magnitude
    float magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }
};

// Function to compute the gravitational force between two bodies
vec3 compute_force(const vec3& pos_i, const vec3& pos_j, float mass_i, float mass_j) {
    vec3 diff = pos_j - pos_i;
    float dist = diff.magnitude() + 1e-10f; // Avoid division by zero
    float F = (G * mass_i * mass_j) / (dist * dist * dist);
    return diff * F;
}

// N-body simulation function with OpenACC acceleration
void nbody_simulation(vec3* positions, vec3* velocities, float* masses, int N) {
    // Run simulation for a fixed number of time steps
    for (int step = 0; step < num_timesteps; ++step) {
        // Array to store forces
        vec3 forces[N];

        // Offload the force computation to the GPU
#pragma acc parallel loop copyin(positions[0:N], masses[0:N]) copyout(forces[0:N])
        for (int i = 0; i < N; ++i) {
            vec3 force(0.0f, 0.0f, 0.0f);
#pragma acc loop reduction(+:force)
            for (int j = 0; j < N; ++j) {
                if (i != j) {
                    vec3 f = compute_force(positions[i], positions[j], masses[i], masses[j]);
                    force = force + f;
                }
            }
            forces[i] = force;
        }

        // Offload the velocity and position update to the GPU
#pragma acc parallel loop copyin(forces[0:N], masses[0:N]) copyinout(velocities[0:N], positions[0:N])
        for (int i = 0; i < N; ++i) {
            vec3 accel = forces[i] * (1.0f / masses[i]);
            velocities[i] = velocities[i] + accel * dt;
            positions[i] = positions[i] + velocities[i] * dt;
        }
    }
}

int main() {
    // Allocate memory for positions, velocities, and masses
    vec3 positions[N];
    vec3 velocities[N];
    float masses[N];

    // Initialize random positions, velocities, and masses
    for (int i = 0; i < N; ++i) {
        positions[i] = vec3(rand() % 100, rand() % 100, rand() % 100);
        velocities[i] = vec3(rand() % 10, rand() % 10, rand() % 10);
        masses[i] = rand() % 100 + 1; // Avoid zero mass
    }

    // Start the N-body simulation with OpenACC acceleration
    nbody_simulation(positions, velocities, masses, N);

    // Output final positions of the first 5 bodies
    std::cout << "Final positions of some bodies:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "Body " << i << ": (" << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << ")\n";
    }

    return 0;
}
