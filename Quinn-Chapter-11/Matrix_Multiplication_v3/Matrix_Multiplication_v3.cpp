#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>

using namespace std;

typedef struct state {
    int rank;
    int size;
    vector<vector<int>> matrixA;
    vector<vector<int>> matrixB;
    vector<vector<int>> matrixC;
    vector<vector<int>> localA;
    int aRows=0, aCols=0, bRows=0, bCols=0;
    int rowsPerProc=0;
    int extra=0;
    int offset = 0;
    vector<int> flatA;
    vector<int> flatB;
    vector<int> flatC;
    vector<int> recvFlatC;
    vector<int> sendCounts;
    vector<int> sendDisplacements;
    vector<int> recvCounts;
    vector<int> recvDisplacements;
} State;

/*
* Read matrix from a text file
*/
void readMatrix(const string& filename, vector<vector<int>>& matrix, int& rows, int& cols) {
    cout << "Reading matix from file " << filename << endl;
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

/*
* Write a matrix to a text file.
*/
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

/*
* Print matrix on screen.
*/
string printMatrix(int rank, string text, const vector<vector<int>>& matrix) {
    char buf[100];
    string sbuf;
    sprintf_s(buf, "printMatrix, rank: %d, text = %s\n", rank, text.c_str());
    sbuf += buf;
    if (matrix.size() != 0) {
        for (size_t i = 0; i < matrix.size(); i++) {
            for (size_t j = 0; j < matrix[i].size(); j++) {
                sprintf_s(buf, "%d, ", matrix[i][j]);
                sbuf += buf;
            }
            sbuf += "\n";
        }
    }
    else {
        sbuf += "** EMPTY **\n";
    }
    //cout << sbuf;
    
    return sbuf;
}

/*
* Print flat matrix (one-dim) on screen.
*/
string printFlatMatrix(int rank, string text, const vector<int> flatMatrix) {
    string sbuf;
    char buf[1024];
    sprintf_s(buf, "printFlatMatrix, rank: %d, text: %s\n", rank, text.c_str());
    sbuf += buf;
    if (flatMatrix.size() != 0) {
        for (size_t i = 0; i < flatMatrix.size(); i++) {
            sprintf_s(buf, "%d, ", flatMatrix[i]);
            sbuf += buf;
        }
    }
    else {
        sbuf += "** EMPTY **";
    }
    sbuf += "\n";
    // cout << sbuf << endl;
    return sbuf;
}

/*
* From a 2-dim matrix to 1-dim matrix
*/
void flattenMatrix(vector<int>& flatMatrix, const vector<vector<int>> matrix) {
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            flatMatrix.push_back(matrix[i][j]);
        }
    }
    return;
}

/*
* From a 1-dim matrix to 2-dim matrix
*/
void unflattenMatrix(vector <vector<int>>& matrix, vector<int> flatMatrix, const int rows, const int cols) {
    char cbuf[1024];
    sprintf_s(cbuf, "unflatten, rows: %d, cols: %d\n", rows, cols);
    cout << cbuf;

    for (size_t i = 0; i < rows; i++) {
        vector<int> row;
        for (size_t j = 0; j < cols; j++) {
            row.push_back(flatMatrix[i * cols + j]);
        }
        matrix.push_back(row);
    }
}

/*
* Print the current state of a process.
* 
*/
void printState(string text, State state) {
    char cbuf[1024];
    string sbuf;
    sprintf_s(cbuf, "\n==> rank: %d, text: %s size: %d\n", state.rank, text.c_str(), state.size);
    sbuf.append(cbuf);
    sprintf_s(cbuf, "aRows: %d, aCols: %d, bRows: %d, bCols: %d\n", state.aRows, state.aCols, state.bRows,
        state.bCols);
    sbuf.append(cbuf);
    sprintf_s(cbuf, "rowsPerProc: %d, extra: %d, offset: %d\n", state.rowsPerProc, state.extra, state.offset);
    sbuf.append(cbuf);
    sbuf.append(printMatrix(state.rank, "printState matrixA", state.matrixA));
    sbuf.append(printMatrix(state.rank, "printState matrixB", state.matrixB));
    sbuf.append(printMatrix(state.rank, "printState matrixC", state.matrixC));
    sbuf.append(printMatrix(state.rank, "printState localA", state.localA));
    sbuf.append(printFlatMatrix(state.rank, "printState flatA", state.flatA));
    sbuf.append(printFlatMatrix(state.rank, "printState flatB", state.flatB));
    sbuf.append(printFlatMatrix(state.rank, "printState flatC", state.flatC));
    sbuf.append(printFlatMatrix(state.rank, "printState recvFlatC", state.recvFlatC));
    sbuf.append(printFlatMatrix(state.rank, "printState sendCounts", state.sendCounts));
    sbuf.append(printFlatMatrix(state.rank, "printState sendDisplacements", state.sendDisplacements));
    sbuf.append(printFlatMatrix(state.rank, "printState recvCounts", state.recvCounts));
    sbuf.append(printFlatMatrix(state.rank, "printState recvDisplacements", state.recvDisplacements));

    cout << sbuf.c_str();
}

int matrixMultiplication(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering.
    MPI_Init(&argc, &argv);

    State state;
    MPI_Comm_rank(MPI_COMM_WORLD, &state.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &state.size);


    if (state.rank == 0) {
        readMatrix("matrixA.txt", state.matrixA, state.aRows, state.aCols);
        readMatrix("matrixB.txt", state.matrixB, state.bRows, state.bCols);

        if (state.aCols != state.bRows) {
            cerr << "Matrix multiplication not possible: Incompatible dimensions" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        state.rowsPerProc = state.aRows / state.size;
        state.extra = state.aRows % state.size;
        state.sendCounts.resize(state.size);
        state.sendDisplacements.resize(state.size);

        for (int i = 0; i < state.size; i++) {
            state.sendCounts[i] = (state.rowsPerProc + (i < state.extra ? 1 : 0)) * state.aCols;
            state.sendDisplacements[i] = state.offset;
            state.offset += state.sendCounts[i];
        }
        flattenMatrix(state.flatA, state.matrixA);
        flattenMatrix(state.flatB, state.matrixB);
        printState("After reads:", state);
    }
    else {
        state.sendCounts.resize(state.size);
        state.sendDisplacements.resize(state.size);
    }
    
    // Brodcasting ...
    MPI_Bcast(&state.aCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&state.bRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&state.bCols, 1, MPI_INT, 0, MPI_COMM_WORLD);    
    MPI_Bcast(state.sendCounts.data(), state.size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(state.sendDisplacements.data(), state.size, MPI_INT, 0, MPI_COMM_WORLD);
    if (state.rank != 0) {
        state.flatB.resize(state.bRows * state.bCols);
        state.flatA.resize(state.sendCounts[state.rank]);
    }
    int localARows = state.sendCounts[state.rank] / state.aCols;
    MPI_Bcast(state.flatB.data(), state.bRows * state.bCols, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (state.rank != 0) {
        unflattenMatrix(state.matrixB, state.flatB, state.bRows, state.bCols);
        
    }

    vector<int> localFlatA(state.sendCounts[state.rank]); 
    printState("After broadcast", state);
    MPI_Scatterv(state.flatA.data(), state.sendCounts.data(), state.sendDisplacements.data(), MPI_INT,
        localFlatA.data(), state.sendCounts[state.rank], MPI_INT, 0, MPI_COMM_WORLD);

    printState("After Scatterv", state);
    unflattenMatrix(state.localA, localFlatA, state.sendCounts[state.rank]/state.aCols, state.aCols);
    

    state.matrixC.resize(localARows);
    for (size_t i = 0; i < state.matrixC.size(); i++) {
        state.matrixC[i].resize(state.bCols, 0);
    }

    for (size_t i = 0; i < state.matrixC.size(); i++) {
        for (size_t j = 0; j < state.matrixC[i].size(); j++) {
            for (size_t k = 0; k < state.aCols; k++) {
                state.matrixC[i][j] += state.localA[i][k] * state.matrixB[k][j];
            }
        }
    }

    printState("After multiplication", state);

    //vector<int> recvCounts(state.size), recvDisplacements(state.size);
    state.recvCounts.resize(state.size);
    state.recvDisplacements.resize(state.size);
    for (int i = 0; i < state.size; i++) {
        state.recvCounts[i] = (state.sendCounts[i] / state.aCols) * state.bCols;
        state.recvDisplacements[i] = (i == 0) ? 0 : state.recvDisplacements[i - 1] + state.recvCounts[i - 1];
    }

    flattenMatrix(state.flatC, state.matrixC);
    
    if (state.rank == 0) {
        state.recvFlatC.resize(state.aRows * state.bCols, 0);
    }

    
    printState("Before Gatherv", state);
    MPI_Gatherv(state.flatC.data(), state.recvCounts[state.rank], MPI_INT, state.recvFlatC.data(),
        state.recvCounts.data(), state.recvDisplacements.data(), MPI_INT, 0, MPI_COMM_WORLD);
    printState("After Gatherv", state);

    if (state.rank == 0) {
        state.matrixC.resize(0);
        unflattenMatrix(state.matrixC, state.recvFlatC, state.aRows, state.bCols);
        writeMatrix("result.txt", state.matrixC);
    }

    printState("Before exit ...", state);

    MPI_Finalize();
    return 0;
}

int main(int argc, char** argv) {
    return matrixMultiplication(argc, argv);
}