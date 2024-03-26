
#include <iostream>
#include <fstream>
#include <string>
#include <queue>

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
        int intervals = process.ServiceTime;
        for (int i = 0; i < intervals; i++) 
        {
            FCFSOutput << process.name << endl;
        }
    
        QueueCopy.pop();
    }


}

int main(int argc, char *argv[])
{
    string Inputfile = argv[1];
    
    ifstream Input("input.in");

    // Check for Input File
    if (!Input) {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    FCFSScheduler(Input);

    return 0;
}