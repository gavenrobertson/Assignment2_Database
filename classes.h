#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

//Dont forget to close the files to avoid mem leaks

class StorageBufferManager {

private:
    
    const int BLOCK_SIZE = 4096; // initialize the  block size allowed in main memory according to the question 

    // You may declare variables based on your need 
    int numRecords = 1;

    // Insert new record 
    void insertRecord(Record record) {

        // No records written yet
        if (numRecords == 0) {
            // Initialize first block

        }
        // Add record to the block


        // Take neccessary steps if capacity is reached (you've utilized all the blocks in main memory)


    }

public:
    StorageBufferManager(string NewFileName) {
        
        //initialize your variables
        ofstream employeeRelationFile("EmployeeRelation");

        // Create your EmployeeRelation file 
        if (employeeRelationFile.is_open()) {
            cout << "The 'EmployeeRelation' file has sucessfully been opened" << endl;
            
        }
        else {
            cerr << "Error creating the 'EmployeeRelation' file!!!" << endl;
        }
    }

    // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
    void createFromFile(string csvFName) {
        
        ifstream csvFile(csvFName);
        if (csvFile.is_open()) {
            cout << "The csvFile was successfully opened!" << endl;
        }
        else {
            cerr << "Error!!! The csvFile could not be opened!" << endl;
        }
        
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};
