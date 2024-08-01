#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <random>
#include "mpi.h"

int Example_0(int argc, char* argv[]);
int Example_1(int argc, char* argv[]);
int Example_2(int argc, char* argv[]);
int Example_3(int argc, char* argv[]);
int Example_4();
int Example_5(int argc, char* argv[]);
std::vector<std::vector<int>> readAdjacencyMatrixFromFile(const std::string filename);


void print_matrix(const float* matrix, int dim);