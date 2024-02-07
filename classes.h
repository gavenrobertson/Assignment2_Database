#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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

    void print() const {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

struct Slot {
    int offset; // Position in page data array
    int length; // Length of the record
};

struct Page {
    static const int PAGE_SIZE = 4096;
    char data[PAGE_SIZE];
    std::vector<Slot> slots;  // Slot directory
    int freeSpaceOffset;      // Offset for the next record

    Page() : freeSpaceOffset(0) {
        memset(data, 0, PAGE_SIZE);
    }

    bool canFit(int recordSize) const {
        int requiredSpace = recordSize + sizeof(Slot);
        int availableSpace = PAGE_SIZE - freeSpaceOffset - (slots.size() * sizeof(Slot));
        return availableSpace >= requiredSpace;
    }

    void addRecord(const string& record) {
        int recordSize = record.size();
        if (!canFit(recordSize)) {
            throw std::runtime_error("Page is full or can't fit the record with the slot");
        }
        memcpy(data + freeSpaceOffset, record.c_str(), recordSize);
        slots.push_back({freeSpaceOffset, recordSize});
        freeSpaceOffset += recordSize;
    }
};

class StorageBufferManager {
private:
    ofstream employeeRelationFile;
    Page currentPage;  // Current page buffer

    void writePageToFile() {
        if (!currentPage.slots.empty()) {
            // Write the data
            employeeRelationFile.write(currentPage.data, currentPage.freeSpaceOffset);
            // Write the slots information
            int numSlots = currentPage.slots.size();
            employeeRelationFile.write(reinterpret_cast<const char*>(&numSlots), sizeof(numSlots));
            for (const Slot& slot : currentPage.slots) {
                employeeRelationFile.write(reinterpret_cast<const char*>(&slot), sizeof(slot));
            }
            currentPage = Page(); // Reset the current page after writing
        }
    }

    void addRecordToPage(const Record& record) {
        stringstream ss;
        ss << record.id << "," << record.name << "," << record.bio << "," << record.manager_id << "\n";
        string recordStr = ss.str();

        if (!currentPage.canFit(recordStr.size())) {
            writePageToFile(); // Write current page to file if the record doesn't fit
        }

        currentPage.addRecord(recordStr); // Add the record to the current page
    }

public:
    StorageBufferManager(string NewFileName) : employeeRelationFile(NewFileName, ios::binary) {
        if (!employeeRelationFile.is_open()) {
            throw runtime_error("Error creating the 'EmployeeRelation' file!!!");
        }
    }

    ~StorageBufferManager() {
        writePageToFile();  // Make sure to write the last page to the file
        employeeRelationFile.close();
    }

    void createFromFile(string csvFName) {
        ifstream csvFile(csvFName);
        if (!csvFile.is_open()) {
            throw runtime_error("Error!!! The csvFile could not be opened!");
        }

        string line;
        while (getline(csvFile, line)) {
            istringstream ss(line);
            vector<string> fields;
            string field;
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            Record record(fields);
            addRecordToPage(record);
        }
    }

    Record findRecordById(int id) {
        cout << "Searching for Record ID: " << id << endl;
        ifstream file("EmployeeRelation");
        if (!file.is_open()) {
            throw runtime_error("Unable to open the file.");
        }

        string line;
        while (getline(file, line)) {
            istringstream ss(line);
            vector<string> fields;
            string field;
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            // Ensure that there are enough fields for a record
            if (fields.size() >= 4) {
                Record record(fields);
                cout << "Checking Record: ID=" << record.id << endl;
                if (record.id == id) {
                    cout << "Found Record: ID=" << record.id << endl;
                    return record;
                }
            } else {
                cout << "Invalid record format: " << line << endl;
            }
        }

        throw std::runtime_error("Record not found");
    }
};