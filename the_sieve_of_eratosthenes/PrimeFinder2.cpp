// the_sieve_of_eratosthenes.cpp : This file contains the 'main' function. Program execution begins and ends there.
/*
    Create a list of natural numbers 2, 3, 4, ..., n, none of which is marked.
    2. Set k to 2, the first unmarked number on the list.
    3. Repeat
        (a) Mark all multiples of k between k2 and n
        (b) Find the smallest number greater thank that is unmarked.Set k to this new value.
        Until k2 > n
    4. The unmarked numbers are primes.

*/
#include "PrimeFinder.h"

PrimeFinder2::PrimeFinder2() {

}

PrimeFinder2::~PrimeFinder2() {

}

std::vector<int> PrimeFinder2::getListOfPrimes(int start, int end) {
    int n = end;
    bool* marked = new bool[n + 1];
    // 1. create a list of 2, 3, 4, ..., n, none of which is marked.
    for (int i = 0; i <= end; i++) {
        marked[i] = false;
    }
    int k = sqrt(start), kk;
    if (k < 2) { k = 2; }
    kk = k * k;

    while (kk <= end) {
        // (a) mark all multiple of k between kk and n
        std::cout << "k:" << k << std::endl;
        for (int i = kk; i <= end; i++) {
            if (i % k == 0) {
                marked[i] = true;
            }
        }
        // (b) find the smallest number greater than k that is unmarked. set k to this new value
        for (int i = k + 1; i <= end; i++) {
            if (!marked[i]) {
                k = i;
                break;
            }
        }
        if (k * k == kk) {
            break;
        }
        kk = k * k;
    }

    // Make a list of primes and return.
    std::vector<int> primes;
    for (int i = start; i <= end; i++) {
        if (!marked[i]) {
            primes.push_back(i);
        }
    }
    delete[] marked;
    return primes;
}

