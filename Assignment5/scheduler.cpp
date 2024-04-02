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
    int remainingTime;

};

 struct CompareServiceTime
    {
        bool operator()(const ProcessName& p1, const ProcessName& p2)
        {
            return p1.ServiceTime > p2.ServiceTime;
        }
    };

void FCFSScheduler(ifstream& inputFile, ofstream& FCFSOutput)
{
    // First Come First Serve Scheduler
    // ofstream FCFSOutput("Myfcfs.out");
    // Create queue for processes
    queue<ProcessName> ProcessQueue;

    string name;
    int arrival;
    int service;

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
    FCFSOutput.close();

}

void RoundRobinScheduler(ifstream& inputFile,int Quantum, ofstream& RROutput)
{

    int currentTime = 0;
    
    string name;
    int arrivalTime;
    int serviceTime;

    if (Quantum == 10)
    {
        int currentTime = 10;
    }
 
    vector<ProcessName> allProcesses;
    queue<ProcessName> readyQueue;
    
    while(inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime, serviceTime};  // Initialize remainingTime to serviceTime
        allProcesses.push_back(process);
    }

    
    int index = 0;
    int timeQuantum = Quantum;

    while (!allProcesses.empty() || !readyQueue.empty()) 
    {
        while (index < allProcesses.size() && allProcesses[index].ArrivalTime <= currentTime) 
        {
            readyQueue.push(allProcesses[index]);
            allProcesses.erase(allProcesses.begin() + index);
        }

        if (!readyQueue.empty()) 
        {
            ProcessName process = readyQueue.front();
            readyQueue.pop();

            int ProcesserTime = min(timeQuantum, process.remainingTime);

            process.remainingTime -= ProcesserTime;
            currentTime += ProcesserTime;

            for (int i = 0; i < ProcesserTime / 10; i++)
            {
            RROutput << process.name << endl;
            }
            if (process.remainingTime > 0)  
            {
                readyQueue.push(process);  
            }
        }
        else 
        {
            currentTime += 10;
        }        
    }

    RROutput.close();
    
}

void ShortestProcessNext(ifstream& inputFile , ofstream& SPNOutput) {
    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessQueue;
    vector<ProcessName> processes;

    string name;
    int arrival;
    int service;
    int remain;

    while(inputFile >> name >> arrival >> service) {
        ProcessName process = {name, arrival, service};
        processes.push_back(process);
    }

    int currentTime = 0;
    size_t index = 0;

    cout << "Inside SPN" << endl;

    while (!processes.empty() || !ProcessQueue.empty() || index < processes.size()) {
        while (index < processes.size() && processes[index].ArrivalTime <= currentTime) {
            ProcessQueue.push(processes[index]);
            index++;
        }

        if (!ProcessQueue.empty()) {
            ProcessName process = ProcessQueue.top();
            ProcessQueue.pop();

            for (int i = 0; i < process.ServiceTime / 10; i++) {
                SPNOutput << process.name << endl;
            }

            currentTime += process.ServiceTime;
        }
        else {
            if (index < processes.size()) {
                currentTime = processes[index].ArrivalTime;
            } else {
                break; // No more processes to execute
            }
        }
    }

    SPNOutput.close();
}


void SRPScheduler(ifstream& inputFile, ofstream& SRPOutput) {
    priority_queue<ProcessName, vector<ProcessName>, CompareServiceTime> ProcessQueue;
    vector<ProcessName> processes;

    string name;
    int arrival;
    int service;

    // Read processes from input file
    while (inputFile >> name >> arrival >> service) {
        ProcessName process = {name, arrival, service, service};
        processes.push_back(process);
    }

    int currentTime = 0;
    size_t index = 0;

    // Continue until there are processes in the queue or waiting
    while (!processes.empty() || !ProcessQueue.empty()) {
        // Add processes to the priority queue that have arrived by the current time
        while (index < processes.size() && processes[index].ArrivalTime <= currentTime) {
            ProcessQueue.push(processes[index]);
            index++;
        }

        // If there's no process in the queue and no more processes to add, exit the loop
        if (ProcessQueue.empty()) {
            if (index >= processes.size()) break;
            currentTime = processes[index].ArrivalTime;
            continue;
        }

        // Get the process with the shortest service time
        ProcessName currentProcess = ProcessQueue.top();
        ProcessQueue.pop();

        // Output the current process
        SRPOutput << currentProcess.name << endl;

        // Update current time and remaining time for the process
        currentTime += currentProcess.ServiceTime;

        // Check if the process has finished execution
        if (currentProcess.ServiceTime > 10) {
            // Decrement the remaining service time for the process
            currentProcess.ServiceTime -= 10;
            // Push the process back into the queue with updated service time
            ProcessQueue.push(currentProcess);
        }
    }

    SRPOutput.close();
}


/*
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

void Feedback(ifstream& inputFile)
{
    ofstream FeedbackOutput("Myfeedback.out");

    queue<ProcessName> ProcessQueue;

    int currentTime = 0;

    while (inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service};
        ProcessQueue.push(process);
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

    ofstream FCFSOutput("Myfcfs.out");
    ofstream RROutput10("Myrr10.out");
    //ofstream RROutput40("Myrr40.out");
    ofstream SPNOutput("Myspn.out");
    ofstream SRPOutput("Mysrt.out");

    // Working
    
    FCFSScheduler(Input, FCFSOutput);

    Input.clear();
    Input.seekg(0, ios::beg);
    
    RoundRobinScheduler(Input,10, RROutput10);

    Input.clear();
    Input.seekg(0, ios::beg);
    SRPScheduler(Input, SRPOutput);

    Input.clear();
    Input.seekg(0, ios::beg);
    ShortestProcessNext(Input, SPNOutput);

    cout << "After SPN" << endl;

    

    // Not working
    //HRRN(Input);
    
    return 0;
}