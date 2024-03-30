#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std;
struct ProcessName
{
    string name;
    int arrivalTime;
    int ServiceTime; 
    int remainingTime;

};

    string name;
    int arrival;
    int service;
    int remain;

 struct CompareServiceTime
    {
        bool operator()(const ProcessName& p1, const ProcessName& p2)
        {
            return p1.ServiceTime > p2.ServiceTime;
        }
    };

void FCFSScheduler(ifstream& inputFile)
{
    // First Come First Serve Scheduler
    ofstream FCFSOutput("Myfcfs.out");
    // Create queue for processes
    queue<ProcessName> ProcessQueue;

    while (inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service};
        ProcessQueue.push(process);
        ProcessName process1 = ProcessQueue.front();
        int intervals = process1.ServiceTime / 10;
        for (int i = 0; i < intervals; i++) 
        {
            FCFSOutput << process1.name << endl;
        }
        ProcessQueue.pop();
    }
}

void RoundRobinScheduler(ifstream& inputFile)
{
    ofstream RROutput("Myrr.out");

    vector<ProcessName> allProcesses;
    queue<ProcessName> readyQueue;
    string name;
    int arrivalTime;
    int serviceTime;

    while(inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime, serviceTime};  // Initialize remainingTime to serviceTime
        allProcesses.push_back(process);
    }

    int currentTime = 0;
    int index = 0;

    while (!allProcesses.empty() || !readyQueue.empty()) 
    {
        while (index < allProcesses.size() && allProcesses[index].arrivalTime <= currentTime) 
        {
            readyQueue.push(allProcesses[index]);
            allProcesses.erase(allProcesses.begin() + index);
        }

        if (!readyQueue.empty()) 
        {
            ProcessName process = readyQueue.front();
            readyQueue.pop();

            process.remainingTime -= 1;  // Decrease the remaining time by one
            RROutput << process.name << endl;

            if (process.remainingTime > 0)  // If the process has not finished
            {
                readyQueue.push(process);  // Add it back to the end of the queue
            }
        }

        currentTime += 1;  // Increase the current time by one
    }
}
/*
void ShortestProcessNext(ifstream& inputFile)
{
    // Shortest Process Next Scheduler
    ofstream SPNOutput("Myspn.out");

    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessQueue;
    vector<ProcessName> processes;

    string name;
    int arrivalTime;
    int serviceTime;

    while(inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime};
        processes.push_back(process);
    }

    int currentTime = 0;
    int index = 0;

    while (!processes.empty() || !ProcessQueue.empty()) 
    {
        while (index < processes.size() && processes[index].ArrivalTime <= currentTime) 
        {
            ProcessQueue.push(processes[index]);
            index++;
        }

        if (!ProcessQueue.empty()) 
        {
            ProcessName process = ProcessQueue.top();
            ProcessQueue.pop();

            for (int i = 0; i < process.ServiceTime / 10; i++) 
            {
                SPNOutput << process.name << endl;
            }

            currentTime += process.ServiceTime;
        }
        else 
        {
            currentTime = processes[index].ArrivalTime;
        }
    }
}

void SRPScheduler(ifstream& inputFile)
{
    ofstream SRPOutput("Mysrt.out");

    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessQueue;
    vector<ProcessName> processes;

    string name;
    int arrivalTime;
    int serviceTime;

    while(inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime};
        processes.push_back(process);
    }

    int currentTime = 0;
    int index = 0;

    while (!processes.empty() || !ProcessQueue.empty()) 
    {
        while (index < processes.size() && processes[index].ArrivalTime <= currentTime) 
        {
            ProcessQueue.push(processes[index]);
            index++;
        }

        if (!ProcessQueue.empty()) 
        {
            ProcessName process = ProcessQueue.top();
            ProcessQueue.pop();

            for (int i = 0; i < process.ServiceTime / 10; i++) 
            {
                SRPOutput << process.name << endl;
            }

            currentTime += process.ServiceTime;
        }
        else 
        {
            currentTime = processes[index].ArrivalTime;
        }
    }
}

void HRRN(ifstream& inputFile)
{
    ofstream HRRNOutput("Myhrrn.out");

    queue<ProcessName> ProcessQueue;


    int currentTime = 0;

    while (inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service};
        ProcessQueue.push(process);

        ProcessName process1 = ProcessQueue.front();
        int intervals = process1.ServiceTime / 10;


    }



}
*/

int main()
{

    string Inputfile = "input.in";
    
    ifstream Input(Inputfile);
    // Check for Input File
    if (!Input) {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    // Working 
    //ShortestProcessNext(Input);
    //FCFSScheduler(Input);
    //SRPScheduler(Input);
    // Not working
    RoundRobinScheduler(Input);
    
    //HRRN(Input);

    return 0;
}