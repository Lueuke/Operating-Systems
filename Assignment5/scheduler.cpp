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

void ShortestProcessNext(ifstream& inputFile , ofstream& SPNOutput)
{
    
    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessQueue;
    vector<ProcessName> processes;

    string name;
    int arrival;
    int service;
    int remain;

    while(inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service};
        processes.push_back(process);
    }

    int currentTime = 0;
    int index = 0;

    cout << "Inside SPN" << endl;

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
return;
}

void SRPScheduler(ifstream& inputFile , ofstream& SRPOutput)
{
    
    string name;
    int arrival;
    int service;
    int remain;

    priority_queue<ProcessName, deque<ProcessName>, CompareServiceTime> ProcessQueue;
    vector<ProcessName> processes;

    while(inputFile >> name >> arrival >> service) 
    {
        ProcessName process = {name, arrival, service};
        processes.push_back(process);
    }

    int currentTime = 0;
    int index = 0;

    return;
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
            if (index < processes.size())
            {
                currentTime = processes[index].ArrivalTime;
            }
        }
       
    }
    return;
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

    // SRPS is and SPN is running forever 
    Input.clear();
    Input.seekg(0, ios::beg);
    SRPScheduler(Input, SRPOutput);
    
    cout << "After SPRS" << endl;

    Input.clear();
    Input.seekg(0, ios::beg);
    ShortestProcessNext(Input, SPNOutput);

    cout << "After SPN" << endl;

    

    // Not working
    //HRRN(Input);
    
    return 0;
}