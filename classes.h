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

    vector<char> serialize() const {
        vector<char> data;


        for (int i = 7; i >= 0; --i) {
            data.push_back((id >> (i * 8)) & 0xFF);
            data.push_back((manager_id >> (i * 8)) & 0xFF);
        }

        int nameLength = name.size();
        int bioLength = bio.size();

        data.push_back((nameLength >> 8) & 0xFF);
        data.push_back(nameLength & 0xFF);

        data.push_back((bioLength >> 8) & 0xFF);
        data.push_back(bioLength & 0xFF);

        data.insert(data.end(), name.begin(), name.end());
        data.insert(data.end(), bio.begin(), bio.end());

        return data;
    }


};

//Don't forget to close the files to avoid mem leaks

class StorageBufferManager {

private:
    ofstream employeeRelationFile;
    const int BLOCK_SIZE = 4096; // initialize the  block size allowed in main memory according to the question
    int numRecords = 0;
    vector<char> currentPage;

    //writing the page to the file.
    void writePageToFile(){
        if (!currentPage.empty()){
            employeeRelationFile.write(&currentPage[0], currentPage.size());
            currentPage.clear();
        }
    }

    // Insert new record 
    void addRecordToPage(Record record) {
        vector<char> serializedRecord = record.serialize();
        int recordSize = serializedRecord.size();

        if (currentPage.size() + recordSize > BLOCK_SIZE) {
            writePageToFile();
        }

         currentPage.insert(currentPage.end(), serializedRecord.begin(), serializedRecord.end());
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
        writePageToFile();
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
            addRecordToPage(record);

        }

        
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};
