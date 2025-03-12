//#include <iostream>
//#include <vector>
//#include <limits>
//#include <mpi.h>
//
//using namespace std;
//
//const int INF = numeric_limits<int>::max();
//
//int main(int argc, char** argv) {
//    int rank, size;
//    MPI_Init(&argc, &argv);
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//
//    int n; // Size of the graph
//    vector<vector<int>> graph;
//
//    if (rank == 0) {
//        // Read the graph from input (replace with your input method)
//        cin >> n;
//        graph.resize(n, vector<int>(n));
//        for (int i = 0; i < n; ++i) {
//            for (int j = 0; j < n; ++j) {
//                cin >> graph[i][j];
//                if (graph[i][j] == -1) {
//                    graph[i][j] = INF;
//                }
//            }
//        }
//    }
//
//    // Broadcast the graph size
//    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//    // Allocate memory for local matrix
//    int rows_per_process = n / size;
//    int start_row = rank * rows_per_process;
//    int end_row = (rank + 1) * rows_per_process;
//    vector<vector<int>> local_graph(rows_per_process, vector<int>(n));
//
//    // Scatter the graph data
//    MPI_Scatter(graph[0].data(), rows_per_process * n, MPI_INT, local_graph[0].data(), rows_per_process * n, MPI_INT, 0, MPI_COMM_WORLD);
//
//    for (int k = 0; k < n; ++k) {
//        // Broadcast the k-th row
//        vector<int> k_row(n);
//        if (k >= start_row && k < end_row) {
//            k_row = local_graph[k - start_row];
//        }
//        MPI_Bcast(k_row.data(), n, MPI_INT, k / rows_per_process, MPI_COMM_WORLD);
//
//        // Update local matrix
//        for (int i = 0; i < rows_per_process; ++i) {
//            for (int j = 0; j < n; ++j) {
//                if (local_graph[i][k] != INF && k_row[j] != INF) {
//                    local_graph[i][j] = min(local_graph[i][j], local_graph[i][k] + k_row[j]);
//                }
//            }
//        }
//    }
//
//    // Gather the results
//    MPI_Gather(local_graph[0].data(), rows_per_process * n, MPI_INT, graph[0].data(), rows_per_process * n, MPI_INT, 0, MPI_COMM_WORLD);
//
//    // Print the result (if rank 0)
//    if (rank == 0) {
//        // Print the result matrix
//        for (int i = 0; i < n; ++i) {
//            for (int j = 0; j < n; ++j) {
//                if (graph[i][j] == INF) {
//                    cout << "INF ";
//                }
//                else {
//                    cout << graph[i][j] << " ";
//                }
//            }
//            cout << endl;
//        }
//    }
//
//    MPI_Finalize();
//    return 0;
//}



 
#include <mpi.h>
#include <iostream>
#include <vector>
#include <limits>

const int INF = std::numeric_limits<int>::max();  // Infinity

void floydWarshallMPI(int* dist, int n, int rank, int size) {
    int rows_per_proc = n / size;  // Number of rows assigned to each process
    int* local_dist = new int[rows_per_proc * n];

    // Scatter the rows of the distance matrix to all processes
    MPI_Scatter(dist, rows_per_proc * n, MPI_INT, local_dist, rows_per_proc * n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int k = 0; k < n; ++k) {
        int* row_k = new int[n];  // Buffer to hold the k-th row

        // Root process broadcasts the k-th row to all processes
        if (rank == 0) {
            for (int i = 0; i < n; ++i) {
                row_k[i] = dist[k * n + i];
            }
        }

        MPI_Bcast(row_k, n, MPI_INT, 0, MPI_COMM_WORLD);

        // Each process updates its portion of the matrix
        for (int i = 0; i < rows_per_proc; ++i) {
            for (int j = 0; j < n; ++j) {
                int ij = i * n + j;
                if (local_dist[ij] != INF && row_k[j] != INF && local_dist[ij] > local_dist[i * n + k] + row_k[j]) {
                    local_dist[ij] = local_dist[i * n + k] + row_k[j];
                }
            }
        }

        delete[] row_k;
    }

    // Gather the updated distance matrix from all processes
    MPI_Gather(local_dist, rows_per_proc * n, MPI_INT, dist, rows_per_proc * n, MPI_INT, 0, MPI_COMM_WORLD);

    delete[] local_dist;
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

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 4;  // Number of vertices
    int dist[16] = {
        0, 3, INF, 7,
        8, 0, 2, INF,
        5, INF, 0, 1,
        2, INF, INF, 0
    };

    if (rank == 0) {
        std::cout << "Initial distance matrix:\n";
        printSolution(dist, n);
    }

    // Run Floyd-Warshall algorithm using MPI
    floydWarshallMPI(dist, n, rank, size);

    if (rank == 0) {
        std::cout << "Shortest distances between every pair of vertices:\n";
        printSolution(dist, n);
    }

    MPI_Finalize();
    return 0;
}
