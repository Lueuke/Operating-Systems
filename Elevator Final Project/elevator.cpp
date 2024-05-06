/*
=============================================================================
Title : elevator.cpp
Description : This program communicates the Dr.Rees API to schedule people on an elevator using multithreading .
Author : Luke Dekan (R#11766388)
Date : 04/26/2024
Version : 1.0
Usage : Compile and run this program using the GNU C++ compiler
Notes : This program requires that the API is active and that the building file is inputed.
C++ Version : C++11
=============================================================================
*/

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <curl/curl.h>
#include <vector>
#include <cmath>

using namespace std;

// Create a Class to Store all the Elevator Information
class Elevator 
{
public:
    string id;
    int lowestFloor;
    int highestFloor;
    int currentFloor;
    int totalCapacity; 
    int currentCapacity;
    bool isAvailable = true;

    // Default constructor
    Elevator() : id(""), lowestFloor(0), highestFloor(0), currentFloor(0), totalCapacity(0), currentCapacity(0) {}

};

// Function prototypes
void inputCommunicationThread();
void outputCommunicationThread();
void schedulerComputationThread(vector<Elevator>& elevators);
void readFile(const string& filename, vector<Elevator>& elevators);
void startSimulation();
void endSimulation();
Elevator* findClosestElevator(int startFloor, int endFloor,vector<Elevator>& elevators);

// Global variables for muxtex , shared data queue
mutex mtx;
queue<pair<string, string>> dataQueue;
string PersonData;

// Write callback function for cURL
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *userp) 
{
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Main function
int main(int argc, char* argv[]) 
{

    // Take building Input and create Elevator vector 
    string buildingFilePath = argv[1];

    vector<Elevator> elevators;

    ifstream Input(buildingFilePath);

    if (!Input) 
    {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    // Read the building configuration data
    readFile(buildingFilePath, elevators);

    // Start the simulation
    startSimulation();

    // Start the threads
    thread inputThread(inputCommunicationThread);
    thread outputThread(outputCommunicationThread);
    thread schedulerThread([&elevators](){ schedulerComputationThread(elevators); });

    // Join the threads
    inputThread.join();
    outputThread.join();
    schedulerThread.join();

    return 0;
}

// Function to read the building input file and store all information in the Elevator vector
void readFile(const string& filename, vector<Elevator>& elevators) 
{
    ifstream Input(filename);

    if (Input.is_open()) 
    {
        string line;

        while (getline(Input, line)) {
            stringstream ss(line);
            Elevator elevator;

            getline(ss, elevator.id, '\t');
            ss >> elevator.lowestFloor;
            ss.ignore();
            ss >> elevator.highestFloor;
            ss.ignore();
            ss >> elevator.currentFloor;
            ss.ignore();
            ss >> elevator.totalCapacity;

            elevators.push_back(elevator);
        }  
    } 
    Input.close();
}


// Start the Simulation
void startSimulation() 
{

    // Make a PUT Request to start the Simulation
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    string baseUrl = "http://localhost:5432";

    // Start the simulation by making a PUT request to /Simulation/start
    string startUrl = baseUrl + "/Simulation/start";
    CURLcode res;

    string responseBuffer;

    if(curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, startUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}


// Function to get the next input from the API
void inputCommunicationThread() 
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    string baseUrl = "http://localhost:5432";

    // Make a GET request to the API to get the next person data
    while (true) 
    {
        // Delay to avoid overloading the API
         this_thread::sleep_for(chrono::milliseconds(100));

        CURLcode res;
        string url = baseUrl + "/NextInput";
        string responseData;

        if (curl) 
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

            res = curl_easy_perform(curl);

            
            if (res != CURLE_OK) 
            {
                cerr << "Error calling API: " << curl_easy_strerror(res) << endl;
            } 
            else 
            {
                // If there is no data sleep
                if (responseData == "NONE") 
                {
                     this_thread::sleep_for(chrono::milliseconds(500));
                } 
                else
                {
                    // Parse the received person data
                    istringstream iss(responseData);
                    string personName, startFloor, endFloor, _;
                    getline(iss, personName, '\t');
                    getline(iss, startFloor, '\t');
                    getline(iss, endFloor, '\t');
                    char newline;
                    iss.get(newline);

                
                    mtx.lock(); 
                    PersonData = responseData;
                    mtx.unlock(); 
                }
            }
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

// Function to output the communication to the API
void outputCommunicationThread() 
{
    
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    // Counter for checking the simulation status
    int counter = 0;

    
    while (true) {
        
        if (!dataQueue.empty()) {
            
            mtx.lock();

            // Get the information of the person and the elevator from the shared queue
            pair<string, string> data = dataQueue.front();
            dataQueue.pop();

            
            mtx.unlock();

            // Make a PUT request to the API to add the person to the elevator
            if(curl) 
            {
                string url = "http://localhost:5432/AddPersonToElevator/" + data.first + "/" + data.second;
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                CURLcode res = curl_easy_perform(curl);
                if (res != CURLE_OK) 
                {
                cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
                }
            }
        }

        // Check the simulation status ocassionally using a counter
        if (counter >= 1000)
         { 
            
            string url = "http://localhost:5432/Simulation/check";
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

           
            std::string readBuffer;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            CURLcode res = curl_easy_perform(curl);
           if (res != CURLE_OK) 
            {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
            }
            else 
            {
                // End Simulation if the simulation is complete
                if (readBuffer == "Simulation is complete.") 
                {
                    endSimulation();
                    break;
                }
            }

            this_thread::sleep_for(std::chrono::seconds(1));
            
            counter = 0;
        }

        counter++;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

// Function to end the simulation
void endSimulation() 
{

    // Make a PUT request to the API to stop the simulation
    CURL *curl = curl_easy_init();
    if(curl) 
    {
        string url = "http://localhost:5432/Simulation/stop";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }
        curl_easy_cleanup(curl);
    }
    exit(0);
}

// Function Scheduler people
void schedulerComputationThread(vector<Elevator>& elevators) 
{

    while (true) 
    {
        
        if (!PersonData.empty()) 
        {
            
            mtx.lock();

            // Parse the received person data
            istringstream iss(PersonData);
            string personName, floors;
            getline(iss, personName, '|'); 
            getline(iss, floors); 

            // Extract start and end floors from the parsed string
            istringstream floorsStream(floors);
            string startFloorStr, endFloorStr;
            getline(floorsStream, startFloorStr, '|'); 
            getline(floorsStream, endFloorStr, '|');

            // Convert start and end floor strings to integers
            int startFloor = stoi(startFloorStr);
            int endFloor = stoi(endFloorStr);

            // Find the elevator closest to the person's start floor
            Elevator* closestElevator = findClosestElevator(startFloor, endFloor, elevators);
            if (closestElevator == nullptr) 
            {
                // skip person
                mtx.unlock();
                continue;
            }

            closestElevator->currentCapacity++;

            // Push the person's name and the elevator's ID to the shared queue for output thread
            dataQueue.push({personName, closestElevator->id});

            // Clear received person data after processing
            PersonData.clear();

            mtx.unlock();

            
        }
    }
}

// Function to find the closest elevator to the person's start floor 
Elevator* findClosestElevator(int startFloor, int endFloor,vector<Elevator>& elevators) 
{
    
    Elevator* bestElevator = nullptr;
    int ShortestDistance = INT_MAX;
    int minCapacity = INT_MAX;
    
    // Find the closest elevator that is available and can reach the person's start floor based off the current elevator's floor and capacity
    for (auto& elevator : elevators) 
    {
        if (!elevator.id.empty() && elevator.isAvailable && elevator.lowestFloor <= endFloor && elevator.highestFloor >= endFloor) 
        {
            int distance = abs(elevator.currentFloor - startFloor);

            if (distance < ShortestDistance || (distance == ShortestDistance && elevator.currentCapacity < minCapacity)) 
            {
                ShortestDistance = distance;
                minCapacity = elevator.currentCapacity;
                bestElevator = &elevator;

            }
        }
    }

    // Either return the closest elevator or the first elevator if no elevator is available
    return bestElevator ? bestElevator : &elevators[0];
}

