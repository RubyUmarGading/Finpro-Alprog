#include "AyamAyam.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;
using json = nlohmann::json;

// Parsing
vector<string> getData(string messageRecv) {
    vector<string> fields;
    string field;
    for (char c : messageRecv) {
        if (c == ':' || c == '-') {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

// Read file into vector<Data>
vector<Data> readFile(string fileName) {
    vector<Data> dataList;
    ifstream file(fileName);
    string line;

    while (getline(file, line)) {
        vector<string> fields = getData(line);
        if (fields.size() != 3) continue;

        Data data;
        data.jam = stoi(fields[0]);
        data.menit = stoi(fields[1]);
        data.level = stoi(fields[2]);
        dataList.push_back(data);
    }

    return dataList;
}

// Compare timestamps
bool isEarlier(const Data& a, const Data& b) {
    if (a.jam != b.jam) return a.jam < b.jam;
    return a.menit < b.menit;
}

// Overflow comparison
int compareOverflow(const Data& a, const Data& b) {
    if (a.level != b.level)
        return b.level - a.level;
    return isEarlier(a, b) ? -1 : 1;
}

// Underflow comparison
int compareUnderflow(const Data& a, const Data& b) {
    if (a.level != b.level)
        return a.level - b.level;
    return isEarlier(a, b) ? -1 : 1;
}

// General-purpose quicksort
void quickSort(vector<Data>& arr, int low, int high, int (*cmp)(const Data&, const Data&)) {
    if (low < high) {
        Data pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (cmp(arr[j], pivot) < 0) {
                ++i;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        int pi = i + 1;
        quickSort(arr, low, pi - 1, cmp);
        quickSort(arr, pi + 1, high, cmp);
    }
}

// Sort and export overflow/underflow
void sortAndSaveOverflowUnderflow(vector<Data>& dataList) {
    vector<Data> waterlevel_dry, waterlevel_overflow;
    for (const Data& d : dataList) {
        if (d.level >= 180) waterlevel_overflow.push_back(d);
        else if (d.level <= 25) waterlevel_dry.push_back(d);
    }

    if (!waterlevel_overflow.empty())
        quickSort(waterlevel_overflow, 0, waterlevel_overflow.size() - 1, compareOverflow);

    if (!waterlevel_dry.empty())
        quickSort(waterlevel_dry, 0, waterlevel_dry.size() - 1, compareUnderflow);

    // Overflow JSON
    json overJ = json::array();
    for (const Data& d : waterlevel_overflow) {
        overJ.push_back({
            {"timestamp", (d.jam < 10 ? "0" : "") + to_string(d.jam) + ":" + (d.menit < 10 ? "0" : "") + to_string(d.menit)},
            {"level", d.level}
        });
    }
    ofstream overFile("overflow.json");
    overFile << overJ.dump(4);
    overFile.close();

    // Underflow JSON
    json underJ = json::array();
    for (const Data& d : waterlevel_dry) {
        underJ.push_back({
            {"timestamp", (d.jam < 10 ? "0" : "") + to_string(d.jam) + ":" + (d.menit < 10 ? "0" : "") + to_string(d.menit)},
            {"level", d.level}
        });
    }
    ofstream underFile("underflow.json");
    underFile << underJ.dump(4);
    underFile.close();
}

// Export all critical levels (<=25 or >=180)
void writeCritical(vector<Data> dataList) {
    json j;

    for (const Data& d : dataList) {
        if (d.level <= 25 || d.level >= 180) {
            j.push_back({
                {"jam", d.jam},
                {"menit", d.menit},
                {"level", d.level}
            });
        }
    }

    ofstream file("critical_data.json");
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    } else {
        cerr << "Failed to write JSON file\n";
    }
}

bool convertToBinary(const string& txtFile, const string& binFile) {
    vector<Data> data = readFile(txtFile);
    ofstream fout(binFile, ios::binary);
    if (!fout.is_open()) {
        cerr << "Failed to open binary output file." << endl;
        return false;
    }

    for (const Data& d : data) {
        fout.write(reinterpret_cast<const char*>(&d), sizeof(Data));
    }

    fout.close();
    cout << "Binary file written to " << binFile << endl;
    return true;
}