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
vector<Elevator> elevators; 


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

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void SimStop()
{
    
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/Simulation/stop");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void InputThread()
{
    
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

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
                int id, startFloor, endFloor, timeTick;
                ss >> id >> Name >> startFloor >> endFloor >> timeTick;
                peopleQueue.push({Name, startFloor, endFloor, timeTick});
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

            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/NextOutput");
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

            string data = person.Name + " " + to_string(person.startFloor) + " " + to_string(person.endFloor) + " " + to_string(person.timeTick);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) 
            {
                cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
            }
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

}

void SchedulerThread()
{
    
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
    
    inputThread.join();
    schedulerThread.join();
    outputThread.join();

    return 0;
}