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
int The_Sieve_of_Eratosthenes(int argc, char* argv[]);

void print_matrix(const float* matrix, int dim);