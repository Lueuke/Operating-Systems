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

    int currentTime = 10;
    
    string name;
    int arrivalTime;
    int serviceTime;
 
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
        currentTime += 10;
        currentProcess.ServiceTime -= 10;

        // Check if the process has finished execution
        if (currentProcess.ServiceTime > 0) {
            // Push the process back into the queue with updated service time
            ProcessQueue.push(currentProcess);
        }

        // Add processes to the priority queue that have arrived by the current time
        while (index < processes.size() && processes[index].ArrivalTime <= currentTime) {
            ProcessQueue.push(processes[index]);
            index++;
        }
    }

    SRPOutput.close();
}

void Feedback(ifstream& Input, ofstream& FeedbackOutput) 
{
    string name;
    int arrival, service;
    queue<ProcessName> priorityQueues[7];
    vector<ProcessName> processes;

    while (Input >> name >> arrival >> service) 
    {
        processes.push_back({name, arrival, service});
    }

    int currentTime = 0;
    int index = 0;
    bool allEmpty = true;

    do {
        // Add processes to the priority queue that have arrived by the current time
        while (index < processes.size() && processes[index].ArrivalTime <= currentTime) 
        {
            priorityQueues[0].push(processes[index]);
            index++;
        }

        
        // Check each priority queue starting from the highest
        for(int priority = 0; priority <= 6; priority++)
        {
            if (!priorityQueues[priority].empty())
            {
                allEmpty = false;
                ProcessName process = priorityQueues[priority].front();
                priorityQueues[priority].pop();
                FeedbackOutput << process.name << endl;

                // Decrement the service time by the quantum
                process.ServiceTime -= 10;

                // If the process is not finished, move it to the next lower-priority queue
                if (process.ServiceTime > 0) 
                {   
                    if (priority < 6) 
                    {
                        priorityQueues[priority + 1].push(process);
                    } 
                    else 
                    {
                        priorityQueues[priority].push(process);
                    }
                }
                break;
            }
        }

        currentTime += 10;
    } while (!allEmpty && index < processes.size());
}

void HRRN(ifstream& inputFile , ofstream& HRRNOutput)
{
    queue<ProcessName> ProcessQueue;
    string name;
    int arrival;
    int service;

    while (inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service};
        ProcessQueue.push(process);
    }

    int currentTime = 0;
    while (!ProcessQueue.empty()) 
    {
        double highestResponseRatio = -1;
        ProcessName selectedProcess;
        queue<ProcessName> tempQueue;

        // Calculate the waiting time and response ratio for each process
        while (!ProcessQueue.empty()) 
        {
            ProcessName process = ProcessQueue.front();
            ProcessQueue.pop();
            tempQueue.push(process);

            int waitingTime = currentTime - process.ArrivalTime;
            double responseRatio = (waitingTime + process.ServiceTime) / (double)process.ServiceTime;

            if (responseRatio > highestResponseRatio) 
            {
                highestResponseRatio = responseRatio;
                selectedProcess = process;
            }
        }

        // Run the selected process and remove it from the queue
        for (int i = 0; i < selectedProcess.ServiceTime / 10; i++)
        {
            HRRNOutput << selectedProcess.name << endl;
        }
        currentTime += selectedProcess.ServiceTime;

        // Restore the original queue, excluding the selected process
        while (!tempQueue.empty()) 
        {
            ProcessName process = tempQueue.front();
            tempQueue.pop();

            if (process.name != selectedProcess.name) 
            {
                ProcessQueue.push(process);
            }
        }
    }
}

int main(int argc, char* argv[])
{

    string Inputfile = "input.in";
    
    ifstream Input(Inputfile);
    // Check for Input File
    if (!Input) {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    ofstream FCFSOutput("fcfs.out");
    ofstream RROutput10("rr_10.out");
    ofstream RROutput40("rr_40.out");
    ofstream SPNOutput("spn.out");
    ofstream SRPOutput("srt.out");
    ofstream HRRNOutput("hrrn.out");
    ofstream FeedbackOutput("Myfeedback");
   
    
    // Working
    
    FCFSScheduler(Input, FCFSOutput);

    Input.clear();
    Input.seekg(0, ios::beg);
    RoundRobinScheduler(Input,10, RROutput10);

    Input.clear();
    Input.seekg(0, ios::beg);
    RoundRobinScheduler(Input,40, RROutput40);

    Input.clear();
    Input.seekg(0, ios::beg);
    SRPScheduler(Input, SRPOutput);

    Input.clear();
    Input.seekg(0, ios::beg);
    ShortestProcessNext(Input, SPNOutput);

    Input.clear();
    Input.seekg(0, ios::beg);
    HRRN(Input, HRRNOutput);
 

    // Not working
   

    Input.clear();
    Input.seekg(0, ios::beg);
    Feedback(Input,FeedbackOutput);   

    return 0;
}