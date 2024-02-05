/*
Skeleton code for storage and buffer management
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Create the EmployeeRelation file from Employee.csv
    StorageBufferManager manager("EmployeeRelation");
    manager.createFromFile("Employee.csv");
    int userinput = 0;


    // Loop to lookup IDs until user is ready to quit
    while (userinput != -1) {
        cout << "What record would you like to find? (If you want to stop type '-1'." << endl;
        cin >> userinput;
        Record foundrecord = manager.findRecordById(userinput);
        foundrecord.print();
    }

    return 0;
}
