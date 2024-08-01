#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <tclap/CmdLine.h>

class PrimeFinder2 {
public:
    PrimeFinder2();
    ~PrimeFinder2();
    std::vector<int> getListOfPrimes(int start, int end);
};
