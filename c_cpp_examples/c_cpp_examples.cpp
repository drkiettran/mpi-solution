
#include <iostream>
#include <stdexcept>
#include "MemoryExample.h"
#include "DistanceMatrixReader.h"
#include "DistanceMatrixWriter.h"
#include "MatrixReader.h"
#include "VectorReader.h"

void memory_example() {
	std::cout << "Start memory testing ..." << std::endl;
	int m = 5, n = 3;
	MemoryExample<int>* me = new MemoryExample<int>();
	int fixed_array[3][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12} };

	int** b = me->create2DArray(m, n);

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			b[i][j] = -1; // i * n + j;
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			printf("%d, ", b[i][j]);
		}
		printf("\n");
	}
	me->destroy2DArray(b);
	delete me;
	std::cout << "Done memory testing ..." << std::endl;
}

/*
* 1. compute number of rows per process.
* 2. create an array that has `numProcesses` entries.
* 3. assign each entry a `numRow` value. 
* 4. if remainer is not zero, add 1 to the first `rem` processes.
* 
* The first process always has the appropriate number of rows (even if the remainder exists).
* Use this knowledge to create the storage needed for the reading operations.
*/
int* makeNumRowsByProcess(int totalRows, int numProcesses) {
	int numRows = totalRows / numProcesses;
	int* numRowsByProcess = new int[numProcesses];
	int rem = totalRows % numProcesses;
	for (int p = 0; p < numProcesses; p++) {
		if (rem > 0) {
			numRowsByProcess[p] = numRows + 1;
			rem--;
		}
		else {
			numRowsByProcess[p] = numRows;
		}
	}
	return numRowsByProcess;
};


void distance_matrix_test_int(const std::string infilename, const std::string outfilename, 
							  int totalRows, int numCols, int numProcesses) {
	std::cout << "Start testing distance matrix reader ... " << infilename << std::endl;
	std::cout << "Start testing distance matrix writer ... " << outfilename << std::endl;

	int* numRowsByProcess = makeNumRowsByProcess(totalRows, numProcesses);
	MemoryExample<int>* me = new MemoryExample<int>();
	int** storage = me->create2DArray(numRowsByProcess[0], numCols);;

	DistanceMatrixReader<int> dmr(infilename);
	DistanceMatrixWriter<int> dmw(outfilename);


	for (int p = 0; p < numProcesses; p++) {
		dmr.readRows(storage, numRowsByProcess[p], numCols);
		for (int row = 0; row < numRowsByProcess[p]; row++) {
			for (int col = 0; col < numCols; col++) {
				std::cout << storage[row][col] << " ";
			}
			std::cout << std::endl;
		}
		// testing writer.
		dmw.writeRows(storage, numRowsByProcess[p], numCols);
	}

	me->destroy2DArray(storage);
	delete me;
	std::cout << "Done testing distance matrix reader ..." << std::endl;
	std::cout << std::flush;
}

void distance_matrix_test_float(const std::string infilename, const std::string outfilename,
	int totalRows, int numCols, int numProcesses) {
	std::cout << "Start testing distance matrix reader ... " << infilename << std::endl;
	std::cout << "Start testing distance matrix writer ... " << outfilename << std::endl;

	int* numRowsByProcess = makeNumRowsByProcess(totalRows, numProcesses);
	MemoryExample<float>* me = new MemoryExample<float>();
	float** storage = me->create2DArray(numRowsByProcess[0], numCols);;

	DistanceMatrixReader<float> dmr(infilename);
	DistanceMatrixWriter<float> dmw(outfilename);


	for (int p = 0; p < numProcesses; p++) {
		dmr.readRows(storage, numRowsByProcess[p], numCols);
		for (int row = 0; row < numRowsByProcess[p]; row++) {
			for (int col = 0; col < numCols; col++) {
				storage[row][col] += 1.0;
				std::cout << storage[row][col] << " ";
			}
			std::cout << std::endl;
		}
		// testing writer.
		dmw.writeRows(storage, numRowsByProcess[p], numCols);
	}

	me->destroy2DArray(storage);
	delete me;
	std::cout << "Done testing distance matrix reader ..." << std::endl;
	std::cout << std::flush;
}

void matrix_test_float(string infilename) {
	cout << "Start testing matrix reader ... " << infilename << endl;
	MatrixReader<float> mr(infilename);
	int m = mr.getNumRows();
	int n = mr.getNumCols();

	MemoryExample<float>* me = new MemoryExample<float>();
	float** storage = me->create2DArray(m, n);

	mr.readRows(storage, m, n);
	for (int r = 0; r < m; r++) {
		for (int c = 0; c < n; c++) {
			cout << storage[r][c] << " ";
		}
		cout << endl;
	}

	me->destroy2DArray(storage);
	delete me;
	std::cout << "Done testing matrix reader ..." << std::endl;
	std::cout << std::flush;
}

void vector_test_float(string infilename) {
	cout << "Start testing vector reader ... " << infilename << endl;
	VectorReader<float> vr(infilename);

	MemoryExample<float>* me = new MemoryExample<float>();
	float* storage = me->createVector(vr.getCount());

	vr.getVector(storage);
	
	for (int c = 0; c < vr.getCount(); c++) {
		cout << storage[c] << " ";
	}
	cout << endl;
	

	me->destroyVector(storage);
	delete me;
	std::cout << "Done testing vector reader ..." << std::endl;
	std::cout << std::flush;
}

int main()
{
	memory_example();
	distance_matrix_test_int("distance_matrix.txt", "distance_matrix_out.txt", 6, 6, 4);
	distance_matrix_test_float("distance_matrix_float_in.txt", "distance_matrix_float_out.txt", 6, 6, 4);
	matrix_test_float("matrix_int.txt");
	vector_test_float("vector_int.txt");
};

