#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <curl/curl.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <condition_variable>
#include <atomic>
using namespace std;

struct Person {
    string Name;
    int id;
    int startFloor;
    int endFloor;
    int timeTick;
};

struct Elevator 
{
    string Name;
    int lowestFloor;
    int highestFloor;
    int currentFloor;
    int totalCapacity;
};

queue<Person> peopleQueue;
queue<Person> outputQueue;
vector<Elevator> elevators; 
atomic<bool> stopAllThreads(false);

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void SimStart()
{
    
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/Simulation/start");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);

     if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void SimStop()
{
    stopAllThreads == true;

    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/Simulation/stop");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void CheckSim()
{
        
        curl_global_init(CURL_GLOBAL_ALL);
    
        CURL* curl = curl_easy_init();
        string readBuffer;

        while(true)
        {   
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/Simulation/check");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
            CURLcode res = curl_easy_perform(curl);
        
            if (res != CURLE_OK) 
            {
                cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
            }
            // If the simulation is complete, stop the simulation In real test we don't need to call this we can just exit(0) 
            else if (readBuffer == "Simulation is complete.") 
            {
                SimStop();
                break;
            }

            readBuffer.clear();
            this_thread::sleep_for(chrono::seconds(3));

        }
        curl_easy_cleanup(curl);
    
}

void InputThread()
{
    
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    int nextPersonID = 1;
    while(true)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/NextInput");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        
        string readBuffer;

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }
        else {
                stringstream ss(readBuffer);
            string Name;
            int startFloor, endFloor, timeTick;
            ss >> Name >> startFloor >> endFloor >> timeTick;

            // Create a new Person object and assign an ID
            Person person;
            person.id = nextPersonID++; // Assign ID and increment for the next person
            person.Name = Name;
            person.startFloor = startFloor;
            person.endFloor = endFloor;
            person.timeTick = timeTick;

            // Add the person to the queue
            peopleQueue.push(person);

            cout << "Processed Input - Name: " << Name << ", Starting Floor: " << startFloor << ", Ending Floor: " << endFloor << endl;
        }

        readBuffer.clear();
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void OutputThread()
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    string readBuffer;

    while(true)
    {
        if (!peopleQueue.empty())
        {
            Person person = peopleQueue.front();
            peopleQueue.pop();
            
            auto assignedElevator = find_if(elevators.begin(), elevators.end(), [&](Elevator a) {
                return a.currentFloor == person.startFloor;
            });

             string putUrl = "http://localhost:5432/AddPersonToElevator/" + to_string(person.id) + "/" + assignedElevator->Name;

            curl_easy_setopt(curl, CURLOPT_URL, putUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            cout << "Adding Person " << person.Name << " to elevator " << assignedElevator->Name << endl;
            string data = person.Name + " " + to_string(person.startFloor) + " " + to_string(person.endFloor) + " " + to_string(person.timeTick);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) 
            {
                cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
            }
        }
    }

}

void SchedulerThread()
{
    while (true) {
        if (!peopleQueue.empty()) {
            Person nextPerson = peopleQueue.front();
            peopleQueue.pop();

            // Find the elevator that can reach the person the fastest
            auto fastestElevator = min_element(elevators.begin(), elevators.end(), [&](Elevator a, Elevator b) {
                return abs(a.currentFloor - nextPerson.startFloor) < abs(b.currentFloor - nextPerson.startFloor);
            });

            // Check if the elevator has enough capacity for the person
            if (fastestElevator->totalCapacity > 0) {
                // Assign the person to the fastest elevator
                fastestElevator->currentFloor = nextPerson.startFloor;
                fastestElevator->totalCapacity--;

                // Push the person to the output queue
                outputQueue.push(nextPerson);
            }

            // Sleep for a while before the next scheduling decision
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
}

int main(int argc, char* argv[]) 
{

    
    ifstream ElevatorFile(argv[1]);

    // Check if input file exists
    if (!ElevatorFile) 
    {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    string line;
    while (getline(ElevatorFile, line)) 
    {
        stringstream ss(line);
        Elevator elevator;

        getline(ss, elevator.Name, '\t');
        ss >> elevator.lowestFloor;
        ss.ignore();
        ss >> elevator.highestFloor;
        ss.ignore();
        ss >> elevator.currentFloor;
        ss.ignore();
        ss >> elevator.totalCapacity;

        elevators.push_back(elevator);
    }

    SimStart();

    thread inputThread(InputThread);
    thread schedulerThread(SchedulerThread);
    thread outputThread(OutputThread);
    thread checkSimulationThread(CheckSim);
    
    inputThread.join();
    schedulerThread.join();
    outputThread.join();
    checkSimulationThread.join();

    return 0;
}