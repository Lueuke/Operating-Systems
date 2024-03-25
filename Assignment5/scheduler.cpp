
#include <iostream>
#include <fstream>

using namespace std;

struct Time {
    string name; 
    int ArriveTime;
    int ServiceTime;
};



int main(int argc, char *argv[])
{
    // Check for Input File
    if (argc != 2) {
        cout << "ERROR No input file" << argv[0] << endl;
        return 1;
    }

   
    return 0;
}