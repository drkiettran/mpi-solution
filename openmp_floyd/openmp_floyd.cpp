#include <iostream>
#include <omp.h>
#include <vector>
#include <limits>

const int INF = std::numeric_limits<int>::max();  // Define infinity

void floydWarshallOpenMP(int* dist, int n) {
#pragma omp parallel for
    for (int k = 0; k < n; ++k) {
#pragma omp parallel for collapse(2)
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i * n + k] != INF && dist[k * n + j] != INF) {
                    dist[i * n + j] = std::min(dist[i * n + j], dist[i * n + k] + dist[k * n + j]);
                }
            }
        }
    }
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

    // Run Floyd-Warshall algorithm using OpenMP
    floydWarshallOpenMP(graph, n);

    // Print the solution
    std::cout << "Shortest distances between every pair of vertices:\n";
    printSolution(graph, n);

    return 0;
}
