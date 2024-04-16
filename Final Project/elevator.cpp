#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
using namespace std;

struct Person {
    string Name;
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
                int startFloor, endFloor, timeTick;
                ss >> Name >> startFloor >> endFloor >> timeTick;
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

    SimStart();

    thread inputThread(InputThread);
    thread schedulerThread(SchedulerThread);
    thread outputThread(OutputThread);
    
    inputThread.join();
    schedulerThread.join();
    outputThread.join();

    return 0;
}