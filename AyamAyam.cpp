#include "AyamAyam.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;
using json = nlohmann::json;

//timestamp comparator
bool isEarlier(const Data& a, const Data& b) {
    if (a.jam != b.jam) return a.jam < b.jam;
    if (a.menit != b.menit) return a.menit < b.menit;
    return a.detik < b.detik;
}

//overflow comparator for quick sort
int compareOverflow(const Data& a, const Data& b) {
    if (a.level != b.level)
        return b.level - a.level;
    return isEarlier(a, b) ? -1 : 1;
}

//underflow/dry comparator for quick sort
int compareUnderflow(const Data& a, const Data& b) {
    if (a.level != b.level)
        return a.level - b.level;
    return isEarlier(a, b) ? -1 : 1;
}

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

    //Save to overflow.json
    json overJ = json::array();
    for (const Data& d : waterlevel_overflow) {
        char timeStr[9];
        sprintf(timeStr, "%02d:%02d:%02d", d.jam, d.menit, d.detik);

        overJ.push_back({
            {"timestamp", timeStr},
            {"level", d.level}
        });
    }    
    ofstream overFile("overflow.json");
    overFile << overJ.dump(4);
    overFile.close();

    //Save to underflow.json
    json underJ = json::array();
    for (const Data& d : waterlevel_dry) {
        char timeStr[9];
        sprintf(timeStr, "%02d:%02d:%02d", d.jam, d.menit, d.detik);

        underJ.push_back({
            {"timestamp", timeStr},
            {"level", d.level}
        });
    } 
    ofstream underFile("underflow.json");
    underFile << underJ.dump(4);
    underFile.close();
}

// Baca data
vector<Data> readLogFile(const string& filename) {
    vector<Data> entries;
    ifstream fin(filename);
    string line;

    while (getline(fin, line)) {
        int h, m, s, level;
        if (sscanf(line.c_str(), "%d:%d:%d-%d", &h, &m, &s, &level) == 4) {
            entries.push_back({h, m, s, level});
        }
    }

    return entries;
}

// Masukkan critical ke JSON
void writeCritical(const vector<Data>& entries, const string& outFile) {
    json j = json::array();
    for (const auto& e : entries) {
        if (e.level <= 25 || e.level >= 180) {
            char timeStr[9];
            sprintf(timeStr, "%02d:%02d:%02d", e.jam, e.menit, e.detik);
            j.push_back({{"timestamp", timeStr}, {"level", e.level}});
        }
    }

    ofstream fout(outFile);
    fout << j.dump(4);
    fout.close();
}

bool convertToBinary(const string& txtFile, const string& binFile) {
    vector<Data> data = readLogFile(txtFile);
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