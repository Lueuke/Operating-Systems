#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
{
     // Check if filename argument is provided
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1; // indicating failure
    }

    // Open the file
    ifstream file(argv[1]);
    
    // Check if file exists and is open
    if (!file.is_open()) {
        cerr << "Error: File '" << argv[1] << "' not found or cannot be opened." << endl;
        return 1; // indicating failure
    }
    
    return 0;
}
