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

    string name;
    int arrival;
    int service;

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

void RoundRobinScheduler(ifstream& inputFile,int timeQuantum)
{
    // Round Robin Scheduler
    ofstream RROutput10("Myrr10.out");
    
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
            process.ServiceTime -= timeQuantum;
            for (int i = 0; i < (timeQuantum / 10); i++) 
            {
                RROutput10 << process.name << endl;
            }
             
            ProcessQueue.push(process);
        }
        else
        {
            for (int i = 0; i < process.ServiceTime / 10 ; i++) 
            {
                RROutput10 << process.name << endl;
                
            }
        }
    }

}

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

        ProcessName process = ProcessQueue.front();
        int intervals = process.ServiceTime / 10;


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

    // Working 
    //ShortestProcessNext(Input);
    //FCFSScheduler(Input);
    //SRPScheduler(Input);
    // Not working
    //RoundRobinScheduler(Input,10);
    
    HRRN(Input);

    return 0;
}