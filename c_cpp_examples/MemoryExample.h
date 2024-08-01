#pragma once
#include <iostream>

template<class T>
class MemoryExample {
public:
	MemoryExample();
	~MemoryExample();
	T** create2DArray(size_t m, size_t n);
	T* createVector(size_t m);
	void destroy2DArray(T** storage);
	void destroyVector(T* vector);
};

template<class T>
MemoryExample<T>::MemoryExample() {};

template<class T>
MemoryExample<T>:: ~MemoryExample() {};

template<class T>
T** MemoryExample<T>::create2DArray(size_t m, size_t n) {
	T* storage;
	T** b;
	storage = (T*)malloc(m * n * sizeof(T));
	b = (T**)malloc(m * sizeof(T*));
	for (int i = 0; i < m; i++) {
		b[i] = &storage[i * n];
	}
	return b;
};

template<class T>
T* MemoryExample<T>::createVector(size_t m) {
	T* storage;
	storage = (T*)malloc(m * sizeof(T));
	return storage;
}

template<class T>
void MemoryExample<T>::destroy2DArray(T** storage) {
	delete *storage;
	delete storage;
}

template<class T>
void MemoryExample<T>::destroyVector(T* vector) {
	delete vector;
}