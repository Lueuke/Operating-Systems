/*
=============================================================================
Title : scheduler.cpp
Description : This program simulates the execution of processes using different scheduling algorithms.
Author : ldekan (R#11766388)
Date : 04/3/2024
Version : 1.0
Usage : Compile and run this program using the GNU C++ compiler
Notes : This example program has no requirements.
C++ Version : C++11
=============================================================================
*/

#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

// Structure to store process information
struct ProcessName
{
    string name;
    int ArrivalTime;
    int ServiceTime; 
    int remainingTime;
};

// Compare Processes for priority queue
 struct CompareServiceTime
    {
        bool operator()(const ProcessName& p1, const ProcessName& p2)
        {
            return p1.ServiceTime > p2.ServiceTime;
        }
    };

// Function Reset the file pointer to the beginning of the file
void ResetFile(ifstream& inputFile)
{
    inputFile.clear();
    inputFile.seekg(0, ios::beg);
}

// Fist Come First Serve Scheduler
void FCFSScheduler(ifstream& inputFile, ofstream& FCFSOutput)
{
    // Setup the queue to store the processes and variables of process information
    queue<ProcessName> ProcessQueue;
    string name;
    int Arrival_Time;
    int Service_Time;

    // Read the input file and append each process to the output file as they arrive
    while (inputFile >> name >> Arrival_Time >> Service_Time) 
    {
        ProcessName process = {name, Arrival_Time, Service_Time};
        ProcessQueue.push(process);
        ProcessName processlist = ProcessQueue.front();

        for (int i = 0; i <  processlist.ServiceTime / 10; i++) 
        {
            FCFSOutput << processlist.name << endl;
        }
        ProcessQueue.pop();
    }
}

// Round Robin Scheduler
void RoundRobinScheduler(ifstream& inputFile,int Quantum, ofstream& RROutput)
{
    // Setup the queue to store the processes and variables of process information as well as a time tracker
    string name;
    int arrivalTime;
    int serviceTime;
    int timeTrack = 10;
    int timeQuantum = Quantum;
    queue<ProcessName> ProcessQueue;
    queue<ProcessName> outputQueue;
    
    // Store the processes in the queue
    while(inputFile >> name >> arrivalTime >> serviceTime) 
    {
        ProcessName process = {name, arrivalTime, serviceTime, serviceTime};  
        ProcessQueue.push(process);
    }

    // Unit the Queue is empty or all processes are executed
    while (!ProcessQueue.empty() || !outputQueue.empty()) 
    {
        // Retrieve the processes that have arrived by the current time
        while (!ProcessQueue.empty() && ProcessQueue.front().ArrivalTime <= timeTrack) 
        {
        outputQueue.push(ProcessQueue.front());
        ProcessQueue.pop();
        }

        // Execute the processes from the output queue 
        if (!outputQueue.empty()) 
        {
            ProcessName process = outputQueue.front();
            outputQueue.pop();

            // Determine the time for the process to run and update time tracker and remaining time of processor 
            int ProcesserTime = min(timeQuantum, process.remainingTime);

            process.remainingTime -= ProcesserTime;
            timeTrack += ProcesserTime;

            // Output the process to the output file
            for (int i = 0; i < ProcesserTime / 10; i++)
            {
                RROutput << process.name << endl;
            }
            // Add the process back to the queue if it has remaining time
            if (process.remainingTime > 0)  
            {
                outputQueue.push(process);  
            }
        }
        // Update the time tracker
        else 
        {
            timeTrack += 10;
        }        
    }
}

// Shortest Process Next Scheduler
void ShortestProcessNext(ifstream& inputFile , ofstream& SPNOutput) 
{   
    // Create the priority queue to store the processes and variables of process information
    string name;
    int arrival;
    int service;
    int timetrack = 0;
    queue<ProcessName> Processes;
    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessPriority;

    // Read the processes from the input file
    while(inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service ,service};
        Processes.push(process);
    }

    // Process the processes in the queue
    while (!Processes.empty() || !ProcessPriority.empty()) 
    {
        // Retrieve the processes that have arrived by the current time
        while (!Processes.empty() && Processes.front().ArrivalTime <= timetrack) 
        {
            ProcessPriority.push(Processes.front());
            Processes.pop();
        }
        // Execute the process with the shortest service time
        if (!ProcessPriority.empty()) 
        {
            ProcessName process = ProcessPriority.top();
            ProcessPriority.pop();

            // Output the process to the output file and update the time tracker
            for (int i = 0; i < process.ServiceTime / 10; i++) 
            {
                SPNOutput << process.name << endl;
            }

            timetrack += process.ServiceTime;
        }
        // Update the time tracker if there are no processes in the queue
        else 
        {
            if (!Processes.empty()) 
            {
                timetrack = Processes.front().ArrivalTime;
            } 
            // Exit the loop if there are no more processes to execute
            else 
            {
                break;
            }
        }
    }
}

// Shortest Remaining Time Scheduler
void SRTcheduler(ifstream& inputFile, ofstream& SRTOutput) 
{
    // Create the priority queue and other process information 
    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessQueue;
    queue<ProcessName> processes;
    string name;
    int arrival;
    int service;
    int timeTrack = 0;

    // Read processes from input file
    while (inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service, service};
        processes.push(process);
    }

    // Process the processes in the queue
    while (!processes.empty() || !ProcessQueue.empty()) 
    {
        // Add processes to the priority queue that have arrived by the current time
        while (!processes.empty() && processes.front().ArrivalTime <= timeTrack) 
        {
            ProcessQueue.push(processes.front());
            processes.pop();
        }

        // If queue is empty, update the time tracker or break from the loop if no more processes to execute
        if (ProcessQueue.empty()) 
        {
            if (!processes.empty()) 
            {
                timeTrack = processes.front().ArrivalTime;
            } 
            else 
            {
                break; 
            }
            continue;
        }

        // Get the shorested Process
        ProcessName currentProcess = ProcessQueue.top();
        ProcessQueue.pop();

        // Output the process to the output file
        SRTOutput << currentProcess.name << endl;

        // Update time track and remaining service time of the process
        timeTrack += 10;
        currentProcess.ServiceTime -= 10;

        // Check if the process is finished 
        if (currentProcess.ServiceTime > 0) 
        {
            // Add the process back to the queue if not finished 
            ProcessQueue.push(currentProcess);
        }

        // Add process to the priority queue 
        while (!processes.empty() && processes.front().ArrivalTime <= timeTrack) 
        {
            ProcessQueue.push(processes.front());
            processes.pop();
        }
    }
}

// Feedback Scheduler
void Feedback(ifstream& Input, ofstream& FeedbackOutput) 
{
    // Setup process information and all the priorty queues we need 
    string name;
    int arrival, service;
    bool allEmpty;
    int timeTrack = 0;
    queue<ProcessName> ProcessPriority[7];
    queue<ProcessName> processes;

    // Add process information to queue
    while (Input >> name >> arrival >> service) 
    {
        processes.push({name, arrival, service, service});
    }

    // While all the priority queues are not empty 
    do {
        allEmpty = true;
        // Add processes to the priority queue 
        while (!processes.empty() && processes.front().ArrivalTime <= timeTrack) 
        {
            ProcessPriority[0].push(processes.front());
            processes.pop();
        }

        // Look at each queue for highest priority process
        for(int PriorityNumber = 0; PriorityNumber <= 6; PriorityNumber++)
        {
            // If the queue is not empty print the process to the outputfile
            if (!ProcessPriority[PriorityNumber].empty())
            {
                allEmpty = false;
                ProcessName process = ProcessPriority[PriorityNumber].front();
                ProcessPriority[PriorityNumber].pop();

                FeedbackOutput << process.name << endl;

                // Lower service time 
                process.ServiceTime -= 10;

                // If the process still has a service time add to a lower priority queue 
                if (process.ServiceTime > 0) 
                {   
                    if (PriorityNumber < 6) 
                    {
                        ProcessPriority[PriorityNumber + 1].push(process);
                    } 
                    // Other wise add to the same queue 
                    else 
                    {
                        ProcessPriority[PriorityNumber].push(process);
                    }
                }
                break;
            }
        }
        // Increase the time tracker
        timeTrack += 10;
    } while (!allEmpty || !processes.empty());
}

// Highest Response Ratio Next Scheduler
void HRRN(ifstream& inputFile , ofstream& HRRNOutput)
{
    // Setup variables and process information and queue
    queue<ProcessName> ProcessQueue;
    ProcessName CurrentProcess;
    string name;
    int arrival;
    int service;
    int timeTrack = 0;

    // Read the processes from the input file
    while (inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service, service};
        ProcessQueue.push(process);
    }

    while (!ProcessQueue.empty()) 
    {
        // Setup the Highest Response Ratio and a Tempory Queue
        double HRR = -1;
        queue<ProcessName> tempQueue;
    
        // Calculate the waiting time and response ratio for each process
        while (!ProcessQueue.empty()) 
        {   
            // Get the process from the queue
            ProcessName process = ProcessQueue.front();
            ProcessQueue.pop();
            tempQueue.push(process);

            // Calculate the waiting time and response ratio
            int waitingTime = timeTrack - process.ArrivalTime;
            double responseRatio = (waitingTime + process.ServiceTime) / process.ServiceTime;

            // Set HRR and the Current Process
            if (responseRatio > HRR) 
            {
                HRR = responseRatio;
                CurrentProcess = process;
            }
        }

        // Output the process to the output file and update timeTrack
        for (int i = 0; i < CurrentProcess.ServiceTime / 10; i++)
        {
            HRRNOutput << CurrentProcess.name << endl;
        }
        timeTrack += CurrentProcess.ServiceTime;

        // Restore the original queue
        while (!tempQueue.empty()) 
        {
            ProcessName process = tempQueue.front();
            tempQueue.pop();

            if (process.name != CurrentProcess.name) 
            {
                ProcessQueue.push(process);
            }
        }
    }
}

// Main Function 
int main(int argc, char* argv[])
{
    // Import the input file 
    string Inputfile = argv[1];
    ifstream Input(Inputfile);

    // Check if input file exists
    if (!Input) 
    {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    // Create output files
    ofstream FCFSOutput("fcfs.out");
    ofstream RROutput10("rr_10.out");
    ofstream RROutput40("rr_40.out");
    ofstream SPNOutput("spn.out");
    ofstream SRTOutput("srt.out");
    ofstream HRRNOutput("hrrn.out");
    ofstream FeedbackOutput("feedback.out");
   
    
    // Call the scheduling algorithms and Reset the File inputs after each algorithm
    FCFSScheduler(Input, FCFSOutput);

    ResetFile(Input);
    RoundRobinScheduler(Input,10, RROutput10);

    ResetFile(Input);
    RoundRobinScheduler(Input,40, RROutput40);

    ResetFile(Input);
    SRTcheduler(Input, SRTOutput);

    ResetFile(Input);
    ShortestProcessNext(Input, SPNOutput);

    ResetFile(Input);
    HRRN(Input, HRRNOutput);
 
    ResetFile(Input);
    Feedback(Input,FeedbackOutput);   

    return 0;
}