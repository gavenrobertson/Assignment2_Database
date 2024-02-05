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

    static Record deserialize(const vector<char>& data, size_t& offset) {
        int id = 0, manager_id = 0, nameLength = 0, bioLength = 0;
        for (int i = 7; i >= 0; --i) {
            id |= (static_cast<int>(data[offset++]) & 0xFF) << (i * 8);
            manager_id |= (static_cast<int>(data[offset++]) & 0xFF) << (i * 8);
        }
        nameLength |= (static_cast<int>(data[offset++]) & 0xFF) << 8;
        nameLength |= (static_cast<int>(data[offset++]) & 0xFF);
        bioLength |= (static_cast<int>(data[offset++]) & 0xFF) << 8;
        bioLength |= (static_cast<int>(data[offset++]) & 0xFF);
        string name(data.begin() + offset, data.begin() + offset + nameLength);
        offset += nameLength;
        string bio(data.begin() + offset, data.begin() + offset + bioLength);
        offset += bioLength;
        return Record({to_string(id), name, bio, to_string(manager_id)});
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

    void addRecord(const vector<char>& record) {
        if (!canFit(record.size())) {
            throw std::runtime_error("Page is full or can't fit the record with the slot");
        }
        memcpy(data + freeSpaceOffset, record.data(), record.size());
        slots.push_back({freeSpaceOffset, static_cast<int>(record.size())});
        freeSpaceOffset += record.size();
    }
};

//Don't forget to close the files to avoid mem leaks

class StorageBufferManager {
private:
    ofstream employeeRelationFile;
    const int BLOCK_SIZE = 4096;
    vector<char> currentPage;
    vector<Slot> slots;

    void writePageToFile() {
        if (!currentPage.empty()) {
            int numSlots = slots.size();
            employeeRelationFile.write(reinterpret_cast<const char*>(&numSlots), sizeof(numSlots));
            for (const Slot& slot : slots) {
                employeeRelationFile.write(reinterpret_cast<const char*>(&slot), sizeof(slot));
            }
            employeeRelationFile.write(&currentPage[0], currentPage.size());
            currentPage.clear();
            slots.clear();
        }
    }

    void addRecordToPage(const Record& record) {
        vector<char> serializedRecord = record.serialize();
        int recordSize = serializedRecord.size();
        if (currentPage.size() + recordSize > BLOCK_SIZE) {
            writePageToFile();
            currentPage.clear();
            slots.clear();
        }
        currentPage.insert(currentPage.end(), serializedRecord.begin(), serializedRecord.end());
        slots.push_back({static_cast<int>(currentPage.size() - recordSize), recordSize});
    }

public:
    StorageBufferManager(string NewFileName) : employeeRelationFile(NewFileName, ios::binary) {
        if (!employeeRelationFile.is_open()) {
            throw runtime_error("Error creating the 'EmployeeRelation' file!!!");
        }
    }

    ~StorageBufferManager() {
        writePageToFile();
        employeeRelationFile.close();
    }

    void createFromFile(string csvFName) {
        ifstream csvFile(csvFName);
        if (!csvFile.is_open()) {
            throw runtime_error("Error opening the CSV file!");
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
        ifstream file("EmployeeRelation", ios::binary);
        while (!file.eof()) {
            int numSlots;
            file.read(reinterpret_cast<char*>(&numSlots), sizeof(numSlots));
            vector<Slot> slots(numSlots);
            file.read(reinterpret_cast<char*>(&slots[0]), sizeof(Slot) * numSlots);

            for (const Slot& slot : slots) {
                vector<char> buffer(slot.length);
                file.seekg(slot.offset, ios::beg);
                file.read(&buffer[0], slot.length);
                size_t offset = 0;
                Record record = Record::deserialize(buffer, offset);
                if (record.id == id) {
                    return record;
                }
            }
        }
        throw std::runtime_error("Record not found");
    }
};
