
#include <iostream>
#include <vector>
#include <thread>
#include <limits>
#include <mutex>

const int INF = std::numeric_limits<int>::max();  // Infinity

// Function to run the core of Floyd-Warshall's algorithm in parallel
void floydWarshallThread(std::vector<std::vector<int>>& dist, int n, int k, int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dist[i][k] != INF && dist[k][j] != INF) {
                dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }
}

// Floyd-Warshall algorithm using multithreading
void floydWarshall(std::vector<std::vector<int>>& dist, int n, int numThreads) {
    for (int k = 0; k < n; ++k) {
        std::vector<std::thread> threads;

        // Divide work between threads
        int chunkSize = n / numThreads;
        for (int t = 0; t < numThreads; ++t) {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? n : start + chunkSize;

            threads.push_back(std::thread(floydWarshallThread, std::ref(dist), n, k, start, end));
        }

        // Join all threads
        for (auto& th : threads) {
            if (th.joinable()) {
                th.join();
            }
        }
    }
}

// Utility function to print the distance matrix
void printSolution(const std::vector<std::vector<int>>& dist) {
    int n = dist.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dist[i][j] == INF)
                std::cout << "INF ";
            else
                std::cout << dist[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Example graph represented as an adjacency matrix
    std::vector<std::vector<int>> graph = {
        {0, 3, INF, INF},
        {2, 0, INF, INF},
        {INF, 7, 0, 1},
        {6, INF, INF, 0}
    };

    int n = graph.size();
    int numThreads = 4;  // Number of threads to use

    // Run Floyd-Warshall algorithm with threading
    floydWarshall(graph, n, numThreads);

    // Print the solution
    std::cout << "Shortest distances between every pair of vertices:\n";
    printSolution(graph);

    return 0;
}
