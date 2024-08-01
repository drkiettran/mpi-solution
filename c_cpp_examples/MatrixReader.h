#pragma once
#include <string>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
using std::ifstream;
using std::string;

template<class T>
class MatrixReader
{
public:
	MatrixReader(string filename);
	~MatrixReader();
	T** readRows(T** storage, int numRows, int numCols);
	int getNumRows() { return m; }
	int getNumCols() { return n; }

private:
	std::ifstream* file = NULL;
	std::string filename = "";
	int m, n;
};

template<class T>
MatrixReader<T>::MatrixReader(string filename) {
	this->filename = filename;
	if (filename.size() > 0) {
		file = new ifstream(filename);
		if (*file) {
			if (!file->is_open()) {
				cout << "Failed to open file: " << filename << endl;
				delete file;
				file = NULL;
			}
			
			*file >> m;
			*file >> n;
			cout << filename << " is open for reading." << endl;
			cout << "m = " << m << "; n = " << n << endl;
		}
	}
};

template<class T>
T** MatrixReader<T>::readRows(T** storage, int numRows, int numCols) {
	if (file == NULL || !file->is_open()) {
		return NULL;
	}

	for (int row = 0; row < numRows; row++) {
		for (int col = 0; col < numCols; col++) {
			try {
				*file >> storage[row][col];
			}
			catch (std::ifstream::failure e) {
				std::cout << "IOException: " << e.what() << std::endl;
			}
		}
	}

	std::cout << numRows << " rows was read." << std::endl;
	return storage;
}

template<class T>
MatrixReader<T>::~MatrixReader() {
	if (file) {
		file->close();
		delete file;
		cout << "MatrixReader ends." << endl;
	}
};