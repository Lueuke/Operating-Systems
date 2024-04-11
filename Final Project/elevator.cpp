#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <curl/curl.h>

using namespace std;

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

void InputThread()
{
    
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    while(true)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5432/NextInput");
        // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        
        string readBuffer;

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
    
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void OutputThread()
{
    
}

int main(int argc, char* argv[]) 
{
    SimStart();

    thread inputThread(InputThread);

    thread outputThread(OutputThread);

    inputThread.join();

    return 0;
}