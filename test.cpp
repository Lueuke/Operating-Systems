#include <iostream>
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to send integer value to a specified URL via PUT
void send_integer_via_put(const std::string& url, int value) {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a curl handle
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        // Convert integer value to string
        std::string data = std::to_string(value);

        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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

// Function to retrieve data from a specified URL via GET and store it as an integer
int retrieve_integer_via_get(const std::string& url) {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a curl handle
    CURL* curl = curl_easy_init();
    int value = 0;
    if (curl) {
        // Set the URL for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Create a string buffer to hold the response data
        std::string buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        
        // Perform the GET request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Convert the received data to an integer
            value = std::stoi(buffer);
        }

        // Cleanup the curl handle
        curl_easy_cleanup(curl);
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return value;
}

int main() {
    std::cout << "Luke Dekan\n";
    std::cout << "R11766388\n";

    // Step 3: Send the integer 3360 to /initialize via PUT
    send_integer_via_put("http://localhost:5000/initialize", 3360);

    // Step 4: Send the integer 4 to /modify via PUT
    send_integer_via_put("http://localhost:5000/modify", 4);

    // Step 5: Retrieve the data in /initialize via a GET command and store the value as an integer
    int value_from_initialize = retrieve_integer_via_get("http://localhost:5000/initialize");
    std::cout << "Value received from /initialize: " << value_from_initialize << std::endl;

    // Step 6: Retrieve the data in /modify via a GET command and store the value as an integer
    int value_from_modify = retrieve_integer_via_get("http://localhost:5000/modify");
    std::cout << "Value received from /modify: " << value_from_modify << std::endl;

    // Step 7: Send the value received from /modify to /initialize via PUT
    send_integer_via_put("http://localhost:5000/initialize", value_from_modify);

    // Step 8: Send the value received from /initialize to /modify via PUT
    send_integer_via_put("http://localhost:5000/modify", value_from_initialize);

    // Step 9: Retrieve the data in /initialize via a GET command and store the value as an integer
    value_from_initialize = retrieve_integer_via_get("http://localhost:5000/initialize");
    std::cout << "Value received from /initialize: " << value_from_initialize << std::endl;

    // Step 10: Retrieve the data in /modify via a GET command and store the value as an integer
    value_from_modify = retrieve_integer_via_get("http://localhost:5000/modify");
    std::cout << "Value received from /modify: " << value_from_modify << std::endl;

    return 0;
}
