#include <iostream>
#include <vector>
#include <thread>

// Function to compute a portion of the matrix-vector multiplication
void matVecMulThread(const std::vector<std::vector<int>>& matrix, const std::vector<int>& vec, std::vector<int>& result, int startRow, int endRow) {
    int cols = vec.size();
    for (int i = startRow; i < endRow; ++i) {
        result[i] = 0;
        for (int j = 0; j < cols; ++j) {
            result[i] += matrix[i][j] * vec[j];
        }
    }
}

void matrixVectorMultiply(const std::vector<std::vector<int>>& matrix, const std::vector<int>& vec, std::vector<int>& result, int numThreads) {
    int rows = matrix.size();
    int rowsPerThread = rows / numThreads;
    std::vector<std::thread> threads;

    // Create threads and assign them portions of the matrix to compute
    for (int t = 0; t < numThreads; ++t) {
        int startRow = t * rowsPerThread;
        int endRow = (t == numThreads - 1) ? rows : startRow + rowsPerThread;
        threads.emplace_back(matVecMulThread, std::ref(matrix), std::ref(vec), std::ref(result), startRow, endRow);
    }

    // Join the threads back to the main thread
    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    // Example matrix and vector
    std::vector<std::vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::vector<int> vec = { 1, 1, 1 };  // Example vector
    int rows = matrix.size();
    int numThreads = 2;  // Number of threads to use

    std::vector<int> result(rows, 0);

    // Perform matrix-vector multiplication using native threads
    matrixVectorMultiply(matrix, vec, result, numThreads);

    // Print the result
    std::cout << "Result vector:\n";
    for (int i = 0; i < rows; ++i) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
