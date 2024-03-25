
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
    

    string Inputfile = argv[1];
    
    ifstream Input(Inputfile);

    // Check for Input File
    if (!Input) {
        cout << "ERROR No input file" << endl;
        return 1;
    }

    string line;


    int NumberOfProcess = 0;  

    while (getline(Input,line)) 
    {  
      NumberOfProcess++;  
    }


   // Time proccessList[NumberOfProcess];


   
    return 0;
}