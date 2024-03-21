#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;

mutex mutexlock;
condition_variable bufferReady;
bool readingFinished = false;

struct Number {
    int value;
    int lineNumber;
};

queue<Number> buffer;

void readThread(const string& Inputfile)
{
    ifstream Input(Inputfile);

    string line;
    int lineNumber = 0;
    
    while (getline(Input, line)) 
    {
        int num = stoi(line);
        {
            lock_guard<mutex> lock(mutexlock);
            buffer.push({num, lineNumber});
            lineNumber++;
            bufferReady.notify_one(); // Notify processing thread that buffer is ready
        }
    }

    Input.close();
    // Notify processing thread that reading is finished
    {
        lock_guard<mutex> lock(mutexlock);
        readingFinished = true;
    }
    bufferReady.notify_one();
}

void processThread()
{
    ofstream evenFile("even.out");
    ofstream oddFile("odd.out");
    ofstream positiveFile("positive.out");
    ofstream negativeFile("negative.out");
    ofstream squareFile("square.out");
    ofstream cubeFile("cube.out");

    while (true) {
        Number num;
        {
            unique_lock<mutex> lock(mutexlock);
            // Wait until buffer is ready or reading is finished
            bufferReady.wait(lock, []{ return !buffer.empty() || readingFinished; });
            if (buffer.empty() && readingFinished) {
                // If buffer is empty and reading is finished, exit the thread
                break;
            }
            if (buffer.empty()) {
                // If buffer is empty, continue waiting
                continue;
            }
            num = buffer.front();
            buffer.pop();
        }

        if (num.value % 2 == 0) {
            lock_guard<mutex> lock(mutexlock);
            evenFile << num.value << endl;
        }
        if (num.value % 2 != 0) {
            lock_guard<mutex> lock(mutexlock);
            oddFile << num.value << endl;
        }
        if (num.value > 0) {
            lock_guard<mutex> lock(mutexlock);
            positiveFile << num.value << endl;
        }
        if (num.value < 0) {
            lock_guard<mutex> lock(mutexlock);
            negativeFile << num.value << endl;
        }
        if (num.value >= 0 && sqrt(num.value) == floor(sqrt(num.value))) {
            lock_guard<mutex> lock(mutexlock);
            squareFile << num.value << endl;
        }
        if (cbrt(num.value) == floor(cbrt(num.value))) {
            lock_guard<mutex> lock(mutexlock);
            cubeFile << num.value << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "ERROR Cannot Open file" << argv[0] << endl;
        return 1;
    }

    string Inputfile = argv[1];

    thread reader(readThread, Inputfile);
    thread processing(processThread);

    reader.join();
    processing.join();

    return 0;
}
