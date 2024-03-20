/*
=============================================================================
Title : Thread.cpp
Description : This Program reads numbers from a file input then sorts them based on different parameters and puts them into a output file using Multithreading.
Author : errees (R#11766388)
Date : 03/19/2024
Version : 1.0
Usage : Compile and run this program using the GNU C++ compiler
Notes : This example program has no requirements.
C++ Version : C++20
=============================================================================
*/

// Import librarys we need
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <cmath>
#include <string>
#include <cstdlib>

using namespace std;

mutex mtx;

bool isEven(int num) {
    return num % 2 == 0;
}

bool isOdd(int num) {
    return num % 2 != 0;
}

bool isPositive(int num) {
    return num > 0;
}

bool isNegative(int num) {
    return num < 0;
}

bool isSquare(int num) {
    return num >= 0 && sqrt(num) == floor(sqrt(num));
}

bool isCube(int num) {
    int cube_root = round(cbrt(abs(num)));
    return cube_root * cube_root * cube_root == num;
}

void processIntegers(const string& inputFile) {
    ifstream input(inputFile);

    // Create the Output files 
    ofstream evenFile("even.out");
    ofstream oddFile("odd.out");
    ofstream positiveFile("positive.out");
    ofstream negativeFile("negative.out");
    ofstream squareFile("square.out");
    ofstream cubeFile("cube.out");

    string line;
    while (getline(input, line)) {
        int num = std::stoi(line);

        if (isEven(num)) {
            mtx.lock();
            evenFile << num << endl;
            mtx.unlock();
        }
        if (isOdd(num)) {
            mtx.lock();
            oddFile << num << endl;
            mtx.unlock();
        }
        if (isPositive(num)) {
            mtx.lock();
            positiveFile << num << endl;
            mtx.unlock();
        }
        if (isNegative(num)) {
            mtx.lock();
            negativeFile << num << endl;
            mtx.unlock();
        }
        if (isSquare(num)) {
            mtx.lock();
            squareFile << num << endl;
            mtx.unlock();
        }
        if (isCube(num)) {
            mtx.lock();
            cubeFile << num << endl;
            mtx.unlock();
        }
    }

    input.close();
    evenFile.close();
    oddFile.close();
    positiveFile.close();
    negativeFile.close();
    squareFile.close();
    cubeFile.close();
}

int main(int argc, char *argv[]) {
    // Check for Input file 
    if (argc != 2) {
        cerr << "ERROR File cannot be found " << endl;
        return 1;
    }

    // Open thte Input File
    string inputFile = argv[1];

    // Start Processing the numbers 
    thread processingThread(processIntegers, inputFile);

    processingThread.join();

    return 0;
}

