#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>

// Constants
const float G = 6.67430e-11f;  // Gravitational constant
const float dt = 0.01f;        // Time step
const int num_steps = 100;     // Number of simulation steps
const int N = 1000;            // Number of bodies
const int num_threads = 4;     // Number of threads to use

// Structure to represent a 3D vector
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // Vector addition
    Vec3 operator+(const Vec3& b) const {
        return Vec3(x + b.x, y + b.y, z + b.z);
    }

    // Vector subtraction
    Vec3 operator-(const Vec3& b) const {
        return Vec3(x - b.x, y - b.y, z - b.z);
    }

    // Scalar multiplication
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Vector magnitude
    float magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }
};

// Mutex for thread synchronization (if necessary)
std::mutex mtx;

// Function to compute the gravitational force between two bodies
Vec3 compute_force(const Vec3& pos_i, const Vec3& pos_j, float mass_i, float mass_j) {
    Vec3 diff = pos_j - pos_i;
    float dist = diff.magnitude() + 1e-10f; // Avoid division by zero
    float force = (G * mass_i * mass_j) / (dist * dist * dist);
    return diff * force;
}

// Function to be executed by each thread
void thread_worker(int start, int end, std::vector<Vec3>& positions, std::vector<Vec3>& velocities,
    std::vector<float>& masses, std::vector<Vec3>& forces) {
    // Loop through each time step
    for (int step = 0; step < num_steps; ++step) {
        // Calculate forces for the assigned bodies
        for (int i = start; i < end; ++i) {
            Vec3 total_force(0.0f, 0.0f, 0.0f);
            for (int j = 0; j < N; ++j) {
                if (i != j) {
                    total_force = total_force + compute_force(positions[i], positions[j], masses[i], masses[j]);
                }
            }
            forces[i] = total_force;
        }

        // Update velocities and positions for the assigned bodies
        for (int i = start; i < end; ++i) {
            Vec3 acceleration = forces[i] * (1.0f / masses[i]);
            velocities[i] = velocities[i] + acceleration * dt;
            positions[i] = positions[i] + velocities[i] * dt;
        }
    }
}

int main() {
    // Initialize positions, velocities, and masses of the bodies
    std::vector<Vec3> positions(N);
    std::vector<Vec3> velocities(N);
    std::vector<Vec3> forces(N);
    std::vector<float> masses(N);

    // Randomly initialize positions, velocities, and masses
    for (int i = 0; i < N; ++i) {
        positions[i] = Vec3(rand() % 100, rand() % 100, rand() % 100);
        velocities[i] = Vec3(rand() % 10, rand() % 10, rand() % 10);
        masses[i] = rand() % 100 + 1; // Ensure mass is non-zero
    }

    // Create a vector of threads
    std::vector<std::thread> threads;

    // Number of bodies each thread will handle
    int bodies_per_thread = N / num_threads;

    // Launch threads
    for (int t = 0; t < num_threads; ++t) {
        int start = t * bodies_per_thread;
        int end = (t == num_threads - 1) ? N : (t + 1) * bodies_per_thread; // Handle remainder for the last thread
        threads.push_back(std::thread(thread_worker, start, end, std::ref(positions), std::ref(velocities),
            std::ref(masses), std::ref(forces)));
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    // Output the final positions of some bodies
    std::cout << "Final positions of the first 5 bodies:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "Body " << i << ": (" << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << ")\n";
    }

    return 0;
}
