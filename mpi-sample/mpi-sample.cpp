// mpi-sample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include "mpi.h"
#include "Examples.h"

int main(int argc, char* argv[]) {
    if (argc < 2)
    {
        printf("Must provide the example number!\n");
        return -1;
    }

    int example_no = atoi(argv[1]);
    switch (example_no)
    {
    case 0: 
        return Example_0(argc, argv);
    case 1:
        return Example_1(argc, argv);
    case 2:
        return Example_2(argc, argv);
    case 3:
        return Example_3(argc, argv);
    case 4:
        return Example_4(); 
    case 5:
        return Example_5(argc, argv);
    case 6:
        return The_Sieve_of_Eratosthenes(argc, argv);

    default:
        printf("Invalid example number %d\n", example_no);
        return -2;
    }
    return Example_1(argc, argv);
}
