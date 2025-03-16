#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>

using namespace std;

void printMatrix(int rank, const vector<vector<int>>& matrix) {
    char buf[100];
    string sbuf;
    sprintf_s(buf, "printMatrix, rank: %d\n", rank);
    sbuf += buf;
    
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            sprintf_s(buf, "%d, ", matrix[i][j]);
            sbuf += buf;
        }
        sbuf += "\n";
    }
    cout << sbuf;
}

void printFlatMatrix(int rank, const vector<int> flatMatrix) {
    string sbuf;
    char buf[50];
    sprintf_s(buf, "printFlatMatrix, rank: %d\n", rank);
    sbuf += buf;
    for (size_t i = 0; i < flatMatrix.size(); i++) {

        sprintf_s(buf, "%d, ", flatMatrix[i]);
        sbuf += buf;
    }
    cout << sbuf << endl;
}

void flattenMatrix(vector<int>& flatMatrix, const vector<vector<int>> matrix) {
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            flatMatrix.push_back(matrix[i][j]);
        }
    }
    return;
}

void unflattenMatrix(vector <vector<int>>& matrix, vector<int> flatMatrix, const int rows, const int cols) {
    for (size_t i = 0; i < rows; i++) {
        vector<int> row;
        for (size_t j = 0; j < cols; j++) {
            row.push_back(flatMatrix[i * cols + j]);
        }
        matrix.push_back(row);
    }
}

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
    cout << "Writing matix to file " << filename << endl;
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

int matrixMultiplication(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering.
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    vector<int> sendCounts(size), sendDisplacements(size);
    vector<vector<int>> A, B, C;
    int A_rows, A_cols, B_rows, B_cols;
    int rowsPerProc;
    int extra;
    int offset = 0;
    vector<int> flatA, flatB, flatC, recvFlatC;

    
    char cbuf[100];

    // Reads in matrices A and B
    if (rank == 0) {
        readMatrix("matrixA.txt", A, A_rows, A_cols);
        readMatrix("matrixB.txt", B, B_rows, B_cols);

        if (A_cols != B_rows) {
            cerr << "Matrix multiplication not possible: Incompatible dimensions" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        rowsPerProc = A_rows / size; // 1
        extra = A_rows % size; // 0

        for (int i = 0; i < size; i++) {
            sendCounts[i] = (rowsPerProc + (i < extra ? 1 : 0)) * A_cols;
            sendDisplacements[i] = offset;
            offset += sendCounts[i]; // 
        }
 
        printFlatMatrix(rank, sendCounts);
        printFlatMatrix(rank, sendDisplacements);

    }
    if (rank == 0) {
        printMatrix(rank, A);
        printMatrix(rank, B);
        flattenMatrix(flatA, A);
        flattenMatrix(flatB, B);
        printFlatMatrix(rank, flatA);
        printFlatMatrix(rank, flatB);
    }


    
    // broadcast flatB
    MPI_Bcast(&A_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(sendCounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(sendDisplacements.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        flatB.resize(B_rows * B_cols);
    }
    MPI_Bcast(flatB.data(), flatB.size(), MPI_INT, 0, MPI_COMM_WORLD);
    
    
    sprintf_s(cbuf, "rank: %d; A_cols: %d; B_rows: %d; B_cols: %d; ", rank, A_cols, B_rows, B_cols);
    cout << cbuf << endl;
    printFlatMatrix(rank, sendCounts);
    printFlatMatrix(rank, sendDisplacements);
    printFlatMatrix(rank, flatB);

    vector<int> localFlatA(sendCounts[rank]);
    vector<vector<int>> localB;

    unflattenMatrix(localB, flatB, B_rows, B_cols);
    
    printMatrix(rank, localB);
    

    MPI_Scatterv(flatA.data(), sendCounts.data(), sendDisplacements.data(), MPI_INT,
                 localFlatA.data(), sendCounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    
    printFlatMatrix(rank, sendCounts);
    vector<vector<int>> localA;
    int localA_rows = sendCounts[rank] / A_cols;
    unflattenMatrix(localA, localFlatA, localA_rows, A_cols);
 

    // Multiply matix.
    vector<vector<int>> localC;
    localC.resize(localA_rows);

    
    sprintf_s(cbuf, "%d. localA_rows: %d\n", rank, localA_rows);
    cout << cbuf;
    sprintf_s(cbuf, "%d. B_cols: %d\n", rank, B_cols);
    cout << cbuf;

    for (size_t i = 0; i < localC.size(); i++) {
        localC[i].resize(B_cols, 0);
    }
    

    for (size_t i = 0; i < localC.size(); i++) {
        for (size_t j = 0; j < localC[i].size(); j++) {
            for (size_t k = 0; k < A_cols; k++) {
                localC[i][j] += localA[i][k]*localB[k][j];
            }
        }
    }

    vector<int> recvCounts(size), recvDisplacements(size);
    for (int i = 0; i < size; i++) {
        recvCounts[i] = (sendCounts[i] / A_cols) * B_cols;
        recvDisplacements[i] = (i == 0) ? 0 : recvDisplacements[i - 1] + recvCounts[i - 1];
    }
    printFlatMatrix(rank, recvCounts);
    printFlatMatrix(rank, recvDisplacements);
    
    printMatrix(rank, localC);
    
    flattenMatrix(flatC, localC);
    printFlatMatrix(rank, flatC);

    if (rank == 0) {
        recvFlatC.resize(A_rows * B_cols);
        sprintf_s(cbuf, "recvflatc size: %d\n", A_rows* B_cols);
        cout << cbuf;
    }
    
    
    MPI_Gatherv(flatC.data(), recvCounts[rank], MPI_INT, recvFlatC.data(),
        recvCounts.data(), recvDisplacements.data(), MPI_INT, 0, MPI_COMM_WORLD);
    
    printFlatMatrix(rank, recvFlatC);

    if (rank == 0) {
        printFlatMatrix(rank, recvFlatC);
        unflattenMatrix(C, recvFlatC, A_rows, B_cols);
        printMatrix(rank, C);
        writeMatrix("result.txt", C);
    }

    MPI_Finalize();
    
    return 0;

}

int main(int argc, char* argv[]) {
    return matrixMultiplication(argc, argv);
}
