#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void readMatrix(const string& filename, vector<vector<int>>& matrix, int& rows, int& cols) {
    cout << "reading matrix ... " << endl << flush;
    ifstream file(filename);
    if (file.is_open()) {
        file >> rows >> cols;
        cout << rows << "; " << cols << endl << flush;
        matrix.resize(rows, vector<int>(cols));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                file >> matrix[i][j];
                cout << matrix[i][j] << "; ";
            }
        }
        cout << "done reading ..." << endl << flush;
        file.close();
    }
    else {
        cerr << "Unable to open file " << filename << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    cout << filename << ": readMatrix ends ...\n" << flush;
}

void writeMatrix(const string& filename, const vector<vector<int>>& matrix, int rows, int cols) {
    ofstream file(filename);
    if (file.is_open()) {
        file << rows << " " << cols << endl << flush;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                file << matrix[i][j] << " ";
            }
            file << endl;
        }
        file.close();
    }
    else {
        cerr << "Unable to open file " << filename << endl << flush;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

void matrixMultiply(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int rows, int cols, int n, int rank, int size) {
    if (rank != 0) {
        cout << rank << ": multiplying begins ... rows: " << rows << "; size: " << size << flush;
    }
    
    int rows_per_proc = rows / size;
    vector<int> sendcounts(size, rows_per_proc * cols);
    vector<int> displs(size);
    for (int i = 0; i < size; ++i) {
        displs[i] = i * rows_per_proc * cols;
    }

    if (rank != 0) {
        cout << "rank 1 \n" << flush;
    }

    vector<int> local_A(rows_per_proc * cols);
    vector<int> local_C(rows_per_proc * n, 0);
    if (rank != 0) {
        cout << rank << ": rows per proc: " << rows_per_proc << "; n: " << n << "; cols: " << cols << endl << flush;
    }

    
    MPI_Scatterv(&A[0][0], sendcounts.data(), displs.data(), MPI_INT, local_A.data(), rows_per_proc * cols, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        cout << rank << ": scattered \n" << flush;
    }

    MPI_Bcast((void*) & B[0][0], cols * n, MPI_INT, 0, MPI_COMM_WORLD);


    for (int i = 0; i < rows_per_proc; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < cols; ++k) {
                if (rank != 0) {
                    cout << rank << ": i: " << i << ", j: " << j << ", k: " << k << endl << flush;
                }
                local_C[i * n + j] += local_A[i * cols + k] * B[k][j];
            }
        }
    }
    if (rank != 0) {
        cout << rank << ": done loop ... " << endl << flush;
    }

    MPI_Allgatherv(local_C.data(), rows_per_proc * n, MPI_INT, &C[0][0], sendcounts.data(), displs.data(), MPI_INT, MPI_COMM_WORLD);
    if (rank != 0) {
        cout << rank << ": multiplying ends... \n" << flush;
    }
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering.
    MPI_Init(&argc, &argv);
    cout << "begins ...\n" << flush;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    cout << "rank:" << rank << "; size:" << size << endl << flush;

    vector<vector<int>> A, B, C;
    int rowsA=0, colsA=0, rowsB=0, colsB=0;

    if (rank == 0) {
        cout << "reading matrices ..." << endl << flush;
        readMatrix("matrixA.txt", A, rowsA, colsA);
        readMatrix("matrixB.txt", B, rowsB, colsB);
        cout << "return from reading ...\n" << flush;
        if (colsA != rowsB) {
            cerr << "Matrix dimensions do not match for multiplication" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        cout << "resizing ...\n" << flush;
        C.resize(rowsA, vector<int>(colsB, 0));
    }

    cout << "broadcasting ...\n" << flush;
    MPI_Bcast(&rowsA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsB, 1, MPI_INT, 0, MPI_COMM_WORLD);



    
    if (rank != 0) {
        A.resize(rowsA, vector<int>(colsA));
        B.resize(rowsB, vector<int>(colsB));
        C.resize(rowsA, vector<int>(colsB, 0));
    }
    
    matrixMultiply(A, B, C, rowsA, colsA, colsB, rank, size);
    

    if (rank == 0) {
        writeMatrix("result.txt", C, rowsA, colsB);
    }

    cout << rank << ": Ending program ... \n" << flush;
    MPI_Finalize();
    return 0;
}