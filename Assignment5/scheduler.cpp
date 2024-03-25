
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Time {
    string names; 
    int ArriveTime;
    int ServiceTime;
};

void StoreNumbers()
{

}


int main(int argc, char *argv[])
{
    // Check for Input File
    if (argc != 2) {
        cout << "ERROR No input file" << argv[0] << endl;
        return 1;
    }

    string Inputfile = argv[1];
    
    ifstream Input(Inputfile);

    string line;

    int lineCount = 0;  

    while (getline(Input,line)) 
    {  // Loop through each line in the file
      lineCount++;  // Incrementing line count for each line read
    }


   // Time proccessList[lineCount];


   
    return 0;
}