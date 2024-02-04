#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <cstring>

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

// This can be used to store metadata on the slots used in the slot directory
struct Slot { 
    int offset; // Position in page data array
    int length;
    bool inUse;
};


struct Page {
    static const int PAGE_SIZE = 4096;
    char data[PAGE_SIZE];
    std::vector<Slot> slots;  // Create a dynamic slot array
    int freeSpaceOffset;

    // Page constructor that sets up the data array
    Page() : freeSpaceOffset(0) {
        memset(data, 0, PAGE_SIZE);
    }

    bool canFit(int recordSize) const {
       // Check to see if a given record will fit  in the page block size
        return PAGE_SIZE - freeSpaceOffset >= recordSize;
    }

    void addRecord(const vector<char>& record) {
        int recordSize = record.size(); // Set a record size var from the record

        // If we can not fit the record to the page, throw an error
        if (!canFit(recordSize)) throw std::runtime_error("Page is full");

        // Check to make sure we don't go over the page size or slot size
        if (freeSpaceOffset + recordSize > PAGE_SIZE - (slots.size() + 1) * sizeof(Slot)) {
            throw std::runtime_error("Not enough space for the record and slot metadata");
        }

        memcpy(data + freeSpaceOffset, record.data(), recordSize);

        // Add a new slot for this record with the given record data
        slots.push_back(Slot{freeSpaceOffset, recordSize, true});

        // Update the free space offset
        freeSpaceOffset += recordSize;
    }
};


//Don't forget to close the files to avoid mem leaks
class StorageBufferManager {
private:
    ofstream employeeRelationFile;
    const int BLOCK_SIZE = 4096; // initialize the block size allowed in main memory
    int numRecords = 0;

    //  <- Serialize the record info, possibly make a function for this? ->

   void insertRecord(const Record& record) {
        // Need to serialize record to make compatible with storing on medium disk

        // Check to see if we need to start a new page or not

        // Add the record to the page using .addRecord method somehow
   }


public:
    StorageBufferManager(string NewFileName) : employeeRelationFile(NewFileName, ios::binary){

        // Create your EmployeeRelation file 
        if (!employeeRelationFile.is_open()) {
            throw runtime_error("Error!!! The NewFileName could not be created!");
        }

    }

    //destructor to close files and ensure nothing is left.
    ~StorageBufferManager() {
        employeeRelationFile.close();
    }


    // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
    void createFromFile(string csvFName) {
        ifstream csvFile(csvFName);

        if (!csvFile.is_open()) {
            throw runtime_error("Error!!! The csvFile could not be opened!");
        }
        //initialize the string to hold the line value
        string line;
        //While loop loops through line by line in the csv file
        while (getline(csvFile, line)) {

            // istringstream ss creates an object that we initialize as a string, we then can use it to perform input
            // operations such as getline().
            istringstream ss(line);

            // vector that holds the fields, plural.
            vector<string> fields;
            // string that holds one field.
            string field;

            //inner while loop loops through each value up to the comma.
            while (getline(ss, field, ',')) {
                //adding it to the "back" vector fields
                fields.push_back(field);
            }

            // The Record constructor is designed to take a vector<string> as an argument, and it initializes
            // the Record object's fields (id, name, bio, manager_id) based on the contents of this vector.
            Record record(fields);

            //Once the page is implemented this is where we add to the page.
            //addRecordToPage(record);

        }

        
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};