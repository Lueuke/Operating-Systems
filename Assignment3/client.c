#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

// Define a struct to store the response buffer and its size
struct Response {
    char* buffer;
    size_t size;
};

// Define the callback function that will receive the response data and store it in a buffer
static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    struct Response* resp = (struct Response*)userdata;
    size_t new_size = resp->size + size * nmemb;
    resp->buffer = realloc(resp->buffer, new_size + 1); // Add an extra byte for the null terminator
    if (resp->buffer == NULL) {
        printf("Error: could not allocate memory\n");
        return 0;
    }
    memcpy(resp->buffer + resp->size, ptr, size * nmemb);
    resp->buffer[new_size] = '\0'; // Add null terminator
    resp->size = new_size;
    return size * nmemb;
}


void init_put() {
  // Initialize libcurl
  curl_global_init(CURL_GLOBAL_ALL);

  // Create a curl handle
  CURL * curl = curl_easy_init();
  if (curl) {
    // Set the CURL based HTTP headers pointer.
    struct curl_slist * headers = NULL;

    // Set the request data to the integer value 1024
    char * data = "1024";

    // Append the JSON content type to the HTTP header.
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Set the required CURL options for a PUT request.
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/initialize");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    // Perform the PUT request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // Cleanup the curl handle
    curl_easy_cleanup(curl);
  }

  // Cleanup libcurl
  curl_global_cleanup();
}

void init_get() {
  // Initialize libcurl
  curl_global_init(CURL_GLOBAL_ALL);

  // Create a new curl handle for the GET request
  CURL * curl = curl_easy_init();
  
  // Create a string buffer to hold the response data
  struct Response resp = { NULL, 0 };
  if (curl) {

    // Set the URL for the GET request
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/initialize");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the callback function to receive the response data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp); // Set the userdata parameter for the callback function

    // Perform the GET request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

	// Print the response body
    printf("\nResponse body: %s\n", resp.buffer);
	free(resp.buffer);

    // Cleanup the curl handle
    curl_easy_cleanup(curl);

  }

  // Cleanup libcurl
  curl_global_cleanup();
}

int main() {
  init_put(); //Call the initializer PUT
  init_get(); //Call the initializer GET
  return 0;
}