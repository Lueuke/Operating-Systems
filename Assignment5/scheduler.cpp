
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <windows.h>

using namespace std;


struct ProcessName
{
    string name;
    int ArrivalTime;
    int ServiceTime; 
};

void FCFSScheduler(ifstream& inputFile)
{
    // First Come First Serve Scheduler
    ofstream FCFSOutput("fcfs.out");
    // Create queue for processes
    queue<ProcessName> ProcessQueue;

    string name;
    int arrivalTime;
    int serviceTime;

    while (inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime};
        ProcessQueue.push(process);
    }

    queue<ProcessName> QueueCopy = ProcessQueue;

    while (!QueueCopy.empty()) 
    {
        ProcessName process = QueueCopy.front();
        int intervals = process.ServiceTime / 10;
        for (int i = 0; i < intervals; i++) 
        {
            FCFSOutput << process.name << endl;
        }
    
        QueueCopy.pop();
    }


}


void RoundRobinScheduler(ifstream& inputFile,int timeQuantum)
{
    // Round Robin Scheduler
    ofstream RROutput10("rr10.out");
    
    queue<ProcessName> ProcessQueue;

    string name;
    int arrivalTime;
    int serviceTime;

    while (inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime};
        ProcessQueue.push(process);
    }

    while (!ProcessQueue.empty()) 
    {
        ProcessName process = ProcessQueue.front();
        ProcessQueue.pop();

        if (process.ServiceTime > timeQuantum)
        {
            for (int i = 0; i < timeQuantum; i += timeQuantum) 
            {
                RROutput10 << process.name << endl;
            }
            ProcessQueue.push(process);
        }
        else
        {
            for (int i = 0; i < process.ServiceTime; i += timeQuantum) 
            {
                RROutput10 << process.name << endl;
            }
        }
    }
}

int main()
{

    string Inputfile = "input.in";
    
    ifstream Input(Inputfile);
    // Check for Input File
    if (!Input) {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    RoundRobinScheduler(Input,10);
    //FCFSScheduler(Input);
    

    return 0;
}