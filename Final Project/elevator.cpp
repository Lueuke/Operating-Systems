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

struct Elevator {
    string id;
    int lowestFloor;
    int highestFloor;
    int currentFloor;
    int totalCapacity;
    bool isAvailable;
    // Default constructor
    Elevator() : lowestFloor(0), highestFloor(0), currentFloor(0), totalCapacity(0) {}

    // Constructor with parameters
    Elevator(const string& id, int lowestFloor, int highestFloor, int currentFloor, int totalCapacity) 
        : id(id), lowestFloor(lowestFloor), highestFloor(highestFloor), currentFloor(currentFloor), totalCapacity(totalCapacity) {}
};

// Function prototypes
void inputCommunicationThread();
void outputCommunicationThread();
void schedulerComputationThread(vector<Elevator>& elevators);
void readBuildingConfiguration(const string& filename, vector<Elevator>& elevators);
void startSimulation();

// Global variables
mutex mtx;
mutex outputMtx;
queue<pair<string, string>> elevatorQueue;
queue<pair<string, string>> sharedQueue; // New variable
string receivedPersonData;



size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}


int main(int argc, char* argv[]) {
    // Check if the building configuration file path is provided as a command line argument
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <path_to_building_file>" << endl;
        return 1;
    }

    // Get the building configuration file path
    string buildingFilePath = argv[1];

    // Vector to store elevator data
    vector<Elevator> elevators;

    // Read the building configuration data
    readBuildingConfiguration(buildingFilePath, elevators);

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

void readBuildingConfiguration(const string& filename, vector<Elevator>& elevators) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
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
        file.close();
    } else {
        cerr << "Error: Unable to open file " << filename << endl;
    }
}

void startSimulation() {
    // Initialize cURL
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    // Set the base URL for the API
    string baseUrl = "http://localhost:5432";

    // Start the simulation by making a PUT request to /Simulation/start
    string startUrl = baseUrl + "/Simulation/start";
    CURLcode res;

    // Buffer to store the response
    string responseBuffer;

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, startUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        // Set up the write callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            cout << "Response from server: " << responseBuffer << endl;

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}





void inputCommunicationThread() {
    // Initialize cURL
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    // Set the base URL for the API
    string baseUrl = "http://localhost:5432";

    // Simulate communication with the elevator simulation OS
    // This loop runs until the simulation is stopped
    while (true) {
        // Simulate delay to avoid overwhelming the system with requests
        this_thread::sleep_for(chrono::milliseconds(100));

        // Initialize cURL request
        CURLcode res;
        string url = baseUrl + "/NextInput";
        string responseData;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

            // Perform the API request
            res = curl_easy_perform(curl);

            // Check for errors
            if (res != CURLE_OK) {
                cerr << "Error calling API: " << curl_easy_strerror(res) << endl;
            } else {
                // Check the received response
                if (responseData == "NONE") {
                     this_thread::sleep_for(chrono::milliseconds(500));
                } else {
                    // Parse the received person data
                    istringstream iss(responseData);
                    string personID, startFloor, endFloor, _;
                    getline(iss, personID, '\t');
                    getline(iss, startFloor, '\t');
                    getline(iss, endFloor, '\t');
                   char newline;
                    iss.get(newline);

                    // Process the person data as needed
                    cout << "Received person data: PersonID: " << personID << ", Start Floor: " << startFloor << ", End Floor: " << endFloor << endl;
                    
                    // Here you can do further processing or pass the data to the scheduler computation thread
                    // For now, let's store it in the shared variable
                    mtx.lock(); // Lock mutex before accessing shared data
                    receivedPersonData = responseData;
                    mtx.unlock(); // Unlock mutex after accessing shared data
                }
            }
        }
    }
    // Clean up cURL
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}



void outputCommunicationThread() {
    // This loop runs until the simulation is stopped
    while (true) {
        // Check if there is any data in the shared queue
        if (!sharedQueue.empty()) {
            // Lock mutex before accessing shared data
            mtx.lock();

            // Get the person ID and elevator ID from the shared queue
            pair<string, string> data = sharedQueue.front();
            sharedQueue.pop();

            // Unlock mutex after accessing shared data
            mtx.unlock();

            // Make a PUT request to the API to add the person to the elevator
            CURL *curl = curl_easy_init();
            if(curl) {
                string url = "http://localhost:5432/AddPersonToElevator/" + data.first + "/" + data.second;
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                CURLcode res = curl_easy_perform(curl);
                if(res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
            }
        }
    }
}

// Function to calculate distance between two floors
int calculateDistance(int floor1, int floor2) {
    return abs(floor1 - floor2);
}

// Function to find the closest available elevator to a person's current floor
string findClosestElevator(int currentFloor, const vector<Elevator>& elevators) {
    string closestElevatorID;
    int minDistance = INT_MAX;
    
    for (const auto& elevator : elevators) {
    // Check if the elevator is valid before accessing its properties
    if (!elevator.id.empty()) {
        if (elevator.isAvailable) {
            int distance = calculateDistance(currentFloor, elevator.currentFloor);
            if (distance < minDistance) {
                minDistance = distance;
                closestElevatorID = elevator.id;
            }
        }
    }
}
    
    return closestElevatorID;
}

void schedulerComputationThread(vector<Elevator>& elevators) {
    // This loop runs until the simulation is stopped
    while (true) {
        // Check if there is any received person data
        if (!receivedPersonData.empty()) {
            // Lock mutex before accessing shared data
            mtx.lock();

            // Parse the received person data
            istringstream iss(receivedPersonData);
            string personID, floors;
            getline(iss, personID, '|'); // Parse person ID
            getline(iss, floors); // Parse floors (start and end)

            // Extract start and end floors from the parsed string
            istringstream floorsStream(floors);
            string startFloorStr, endFloorStr;
            getline(floorsStream, startFloorStr, '|'); // Get start floor
            getline(floorsStream, endFloorStr, '|'); // Get end floor

            // Convert start and end floor strings to integers
            int startFloor = stoi(startFloorStr);
            int endFloor = stoi(endFloorStr);

            // Find the closest available elevator to the person's current floor
            string closestElevatorID = findClosestElevator(startFloor, elevators);

            // Push the person ID and closest elevator ID to the shared queue
            sharedQueue.push({personID, closestElevatorID});

            // Clear received person data after processing
            receivedPersonData.clear();

            // Unlock mutex after accessing shared data
            mtx.unlock();
        }
    }
}
