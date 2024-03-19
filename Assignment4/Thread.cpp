#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
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
    if (!input.is_open()) {
        cerr << "Error: Unable to open input file." << endl;
        return;
    }

    ofstream evenFile("even.out");
    ofstream oddFile("odd.out");
    ofstream positiveFile("positive.out");
    ofstream negativeFile("negative.out");
    ofstream squareFile("square.out");
    ofstream cubeFile("cube.out");

    if (!evenFile.is_open() || !oddFile.is_open() || !positiveFile.is_open() ||
        !negativeFile.is_open() || !squareFile.is_open() || !cubeFile.is_open()) {
        cerr << "Error: Unable to open output files." << endl;
        return;
    }

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
    if (argc != 2) {
        cerr << "ERROR File cannot be found " << endl;
        return 1;
    }

    string inputFile = argv[1];

    thread processingThread(processIntegers, inputFile);

    processingThread.join();

    return 0;
}
