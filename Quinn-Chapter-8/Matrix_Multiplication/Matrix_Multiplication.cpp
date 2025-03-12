#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void readMatrix(const string& filename, vector<vector<int>>& matrix, int& rows, int& cols) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    file >> rows >> cols;
    matrix.resize(rows, vector<int>(cols));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            file >> matrix[i][j];

    file.close();
}

void writeMatrix(const string& filename, const vector<vector<int>>& matrix) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    int rows = matrix.size();
    int cols = (rows > 0) ? matrix[0].size() : 0;
    file << rows << " " << cols << endl;
    for (const auto& row : matrix) {
        for (int val : row)
            file << val << " ";
        file << endl;
    }
    file.close();
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering.
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<vector<int>> A, B, C;
    int A_rows, A_cols, B_rows, B_cols;

    if (rank == 0) {
        readMatrix("matrixA.txt", A, A_rows, A_cols);
        readMatrix("matrixB.txt", B, B_rows, B_cols);

        if (A_cols != B_rows) {
            cerr << "Matrix multiplication not possible: Incompatible dimensions" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&A_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (A_cols != B_rows) {
        MPI_Finalize();
        return 1;
    }

    if (rank != 0) {
        B.resize(B_rows, vector<int>(B_cols));
    }

    for (int i = 0; i < B_rows; i++)
        MPI_Bcast(B[i].data(), B_cols, MPI_INT, 0, MPI_COMM_WORLD);

    int local_rows = A_rows / size;
    int remainder = A_rows % size;
    vector<int> send_counts(size), displacements(size);

    for (int i = 0; i < size; i++) {
        send_counts[i] = (i < remainder) ? (local_rows + 1) * A_cols : local_rows * A_cols;
        displacements[i] = (i == 0) ? 0 : displacements[i - 1] + send_counts[i - 1];
    }

    vector<int> local_A(send_counts[rank]);
    MPI_Scatterv(A.data()->data(), send_counts.data(), displacements.data(), MPI_INT,
        local_A.data(), send_counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    int local_rows_count = send_counts[rank] / A_cols;
    vector<int> local_C(local_rows_count * B_cols, 0);

    for (int i = 0; i < local_rows_count; i++)
        for (int j = 0; j < B_cols; j++)
            for (int k = 0; k < A_cols; k++)
                local_C[i * B_cols + j] += local_A[i * A_cols + k] * B[k][j];

    vector<int> recv_counts(size), recv_displacements(size);
    for (int i = 0; i < size; i++) {
        recv_counts[i] = (send_counts[i] / A_cols) * B_cols;
        recv_displacements[i] = (i == 0) ? 0 : recv_displacements[i - 1] + recv_counts[i - 1];
    }

    vector<int> C_flat(A_rows * B_cols);
    MPI_Allgatherv(local_C.data(), recv_counts[rank], MPI_INT, C_flat.data(),
        recv_counts.data(), recv_displacements.data(), MPI_INT, MPI_COMM_WORLD);

    if (rank == 0) {
        C.resize(A_rows, vector<int>(B_cols));
        for (int i = 0; i < A_rows; i++)
            for (int j = 0; j < B_cols; j++)
                C[i][j] = C_flat[i * B_cols + j];

        writeMatrix("result.txt", C);
    }

    MPI_Finalize();
    return 0;
}
