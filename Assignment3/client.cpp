#include <iostream>
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void init_put() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a curl handle
    CURL* curl = curl_easy_init();
    if (curl) {
	struct curl_slist *headers = NULL;
        // Set the request data to the integer value 1024
        std::string data = "1024";
	
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/initialize");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        // Perform the PUT request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleanup the curl handle
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}
 

void init_get() {  
 
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a new curl handle for the GET request
    // Create a curl handle
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/initialize");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Create a string buffer to hold the response data
        std::string buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	
        // Perform the GET request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleanup the curl handle
        curl_easy_cleanup(curl);

        // Print the response body
        std::cout << "\nResponse body: " << buffer << std::endl;
        
    }

    // Cleanup libcurl
    curl_global_cleanup();
}

int main() {
    init_put();
    init_get();
    return 0;
}

