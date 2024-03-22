/*
=============================================================================
Title : Thread.cpp
Description : This is program takes input from file and process the numbers determined by different parameters and use Multithreading to process them into the output files.
Author : errees (R#11766388)
Date : 03/20/2024
Version : 1.0
Usage : Compile and run this program using the GNU C++ compiler
Notes : This example program has no requirements.
C++ Version : C++20
=============================================================================
*/

// Import librarys
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
using namespace std;

// Create the mutex lock , Condition variable , and Bool to show status of reading 
mutex mutexlock;
condition_variable bufferReady;
bool readingFinished = false;

// Create a Structure that stores the number value and the line number the value is at 
struct Numbers 
{
    int value;
    int lineNumber;
};

// Create a Buffer using a Queue 
queue<Numbers> buffer;

void readingThread(const string& Inputfile)
{
    // Read the Input file
    ifstream Input(Inputfile);

    // Set the Line number and line 
    string line;
    int linepointer = 0;
    
    // Read the file input line by line by increasing the lineNumber and add numbers to buffer 
    while (getline(Input, line)) 
    {
        int num = stoi(line);
        {
            lock_guard<mutex> lock(mutexlock);
            buffer.push({num, linepointer});
            linepointer++;
        }
    }
    // Close the Input File
    Input.close();
    // Notify processing thread that reading is finished
    lock_guard<mutex> lock(mutexlock);
    readingFinished = true;
    bufferReady.notify_one();
    
   
}

void processNumbersThread()
{
    // Create all the output files 
    ofstream evenFile("even.out");
    ofstream oddFile("odd.out");
    ofstream positiveFile("positive.out");
    ofstream negativeFile("negative.out");
    ofstream squareFile("square.out");
    ofstream cubedFile("cube.out");

    while (true) {
        Numbers num;
        {
            unique_lock<mutex> lock(mutexlock);

            // Wait for buffer to finished inputing numbers
            bufferReady.wait(lock, []{ return !buffer.empty() || readingFinished; });

            // Check if buffer is empty and reading is finished 
            if (buffer.empty() && readingFinished) {
                break;
            }
            // Check if buffer if empty and wait if its empty
            if (buffer.empty()) {
                continue;
            }
            // Retreive the numbers from the buffer
            num = buffer.front();
            buffer.pop();
        }

        // Process the Numbers Based of Parameters 

        // Even Numbers
        if (num.value % 2 == 0) 
        {
            lock_guard<mutex> lock(mutexlock);
            evenFile << num.value << endl;
        }
        // Odd Numbers
        if (num.value % 2 != 0) 
        {
            lock_guard<mutex> lock(mutexlock);
            oddFile << num.value << endl;
        }
        // Positive Numbers
        if (num.value > 0) 
        {
            lock_guard<mutex> lock(mutexlock);
            positiveFile << num.value << endl;
        }
        // Negative Numbers
        if (num.value < 0) 
        {
            lock_guard<mutex> lock(mutexlock);
            negativeFile << num.value << endl;
        }
        // Squared Numbers
        if (num.value >= 0 && sqrt(num.value) == floor(sqrt(num.value))) 
        {
            lock_guard<mutex> lock(mutexlock);
            squareFile << num.value << endl;
        }
        // Cubed Numbers
        if (cbrt(num.value)==floor(cbrt(num.value)))
        {
            lock_guard<mutex> lock(mutexlock);
            cubedFile << num.value << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    // Check for Input File
    if (argc != 2) {
        cout << "ERROR No input file" << argv[0] << endl;
        return 1;
    }

    // Get Input file
    string Inputfile = argv[1];

    // Create Reader and Processing Threads
    thread reader(readingThread, Inputfile);
    thread processing(processNumbersThread);

    // Join threads
    reader.join();
    processing.join();

    return 0;
}
