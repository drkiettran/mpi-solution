#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <tclap/CmdLine.h>

class PrimeFinder {
public:
    PrimeFinder();
    ~PrimeFinder();
    std::vector<int> getListOfPrimes(int n);
};

class PrimeFinder2 {
public:
    PrimeFinder2();
    ~PrimeFinder2();
    std::vector<int> getListOfPrimes(int start, int end);
};

