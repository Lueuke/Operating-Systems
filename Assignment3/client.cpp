/*
=============================================================================
Title : client.cpp
Description : This program makes API request using Curl commands.
Author : Luke Dekan (R#11766388)
Date : 2/22/2024
Version : 1.0
Usage : Compile and run this program using the GNU C++ compiler
Notes : This example program has no requirements.
C++ Version : 6.3.0
=============================================================================
*/

#include <iostream>
#include <sstream>
#include <string>
#include <curl/curl.h>

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void init_put(string endpoint, int value) {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Store the first part of the URL since only the endpoint is changing 
    string url = "http://localhost:5000/";

    // Concat the endpoint from the function call to what command we want to use
    url += endpoint;

    // Create a curl handle
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        // Set the request data to the integer value 3360
        string data = to_string(value);

        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());


        // Perform the PUT request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }

        // Cleanup the curl handle
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}
 

int init_get(string endpoint) {  
    int value = 0;
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Store the first part of the URL since only the endpoint is changing 
    string url = "http://localhost:5000/";

    // Concat the endpoint from the function call to what command we want to use
    url += endpoint;

    // Create a new curl handle for the GET request
    // Create a curl handle
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Create a string buffer to hold the response data
        string buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    
        // Perform the GET request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }
        
        // Store the value from the WRITEDATE into value to return 
	    istringstream(buffer) >> value;

        // Cleanup the curl handle
        curl_easy_cleanup(curl);

    }

    // Cleanup libcurl
    curl_global_cleanup();

    return value;
}

int main() {
    // Print Name and R#
    cout << "Luke Dekan\n";
    cout << "R11766388\n";

    // Sent the first Two Values 3360 and 4 Into PUt 
    init_put("initialize", 3360);
    init_put("modify", 4);

    // Retrieve the Data from GET and Store the values for printing  
    int get_value = init_get("initialize");
    int modify_value = init_get("modify");

    // Print Values
    cout << "\n3360 Value from Initialize via GET: " << get_value << endl;
    cout << "4 Value from modify via GET: " << modify_value << endl;

    // Take the values from the GET put them back into the corresponding PUT command
    init_put("initialize", modify_value);
    init_put("modify", get_value);

    // Retrieve the Data from GET and Store the values for printing 
    get_value = init_get("initialize");
    modify_value = init_get("modify");

    // Print Values
    cout << "\nValue from Initialize via GET: " << get_value << endl;
    cout << "Value from modify via GET: " << modify_value << endl;

    return 0;
}
