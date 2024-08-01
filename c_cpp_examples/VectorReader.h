#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

template<class T>
class VectorReader
{
public:
	VectorReader(string filename);
	~VectorReader();
	T* getVector(T* storage) { copy(v.begin(), v.end(), storage); return storage; };
	int getCount() { return v.size(); }

private:
	std::ifstream* file = NULL;
	std::string filename = "";
	vector<T> v;
};



template<class T>
VectorReader<T>::VectorReader(string filename) {
	this->filename = filename;
	if (filename.size() > 0) {				
		try {
			file = new ifstream(filename);
			if (!file->is_open()) {
				cout << "Failed to open file: " << filename << endl;
				delete file;
				file = NULL;
			}

			T val;
			while (*file >> val) {
				v.push_back(val);
			}
		}
		catch (std::ifstream::failure e) {
			std::cout << "IOException: " << e.what() << std::endl;
		}

	}
};

template<class T>
VectorReader<T>::~VectorReader() {
	if (file) {
		file->close();
		delete file;
		cout << "VectorReader ends." << endl;
	}
};