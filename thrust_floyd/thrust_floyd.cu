#include <iostream>
#include <vector>
#include <thrust/device_vector.h>
#include <thrust/for_each.h>
#include <thrust/execution_policy.h>

const int INF = 1e9;  // Use a large value for infinity

// Functor to update the distance matrix in parallel
struct floyd_warshall_functor {
    int* d_dist;
    int n;
    int k;

    floyd_warshall_functor(int* _dist, int _n, int _k)
        : d_dist(_dist), n(_n), k(_k) {}

    __host__ __device__
        void operator()(int idx) {
        int i = idx / n;
        int j = idx % n;

        int ik = i * n + k;
        int kj = k * n + j;
        int ij = i * n + j;

        if (d_dist[ik] != INF && d_dist[kj] != INF) {
            d_dist[ij] = min(d_dist[ij], d_dist[ik] + d_dist[kj]);
        }
    }
};

void floydWarshallThrust(int* dist, int n) {
    // Move the distance matrix to the GPU
    thrust::device_vector<int> d_dist(dist, dist + n * n);

    for (int k = 0; k < n; ++k) {
        // Use thrust::for_each to update all pairs (i, j)
        thrust::for_each(thrust::device,
            thrust::counting_iterator<int>(0),
            thrust::counting_iterator<int>(n * n),
            floyd_warshall_functor(thrust::raw_pointer_cast(d_dist.data()), n, k));
    }

    // Copy the result back to the host
    thrust::copy(d_dist.begin(), d_dist.end(), dist);
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

    // Run Floyd-Warshall algorithm using Thrust
    floydWarshallThrust(graph, n);

    // Print the solution
    std::cout << "Shortest distances between every pair of vertices:\n";
    printSolution(graph, n);

    return 0;
}
