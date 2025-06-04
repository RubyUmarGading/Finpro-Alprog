#include <iostream>
#include <string>
#include <winsock2.h>
#include <bits/stdc++.h>
#include <fstream>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

#define PORT 8888

struct Data {
    int jam;
    int menit;
    int level;
};

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

// baca data
vector<Data> readFile(string fileName) {
    vector<Data> dataList;
    ifstream file(fileName);
    string line;

    while (getline(file, line)) {
        vector<string> fields = getData(line);
        
        Data data;
        data.jam = stoi(fields[0]);
        data.menit = stoi(fields[1]);
        data.level = stoi(fields[2]);
        dataList.push_back(data);
        
    }

    return dataList;
}
//timestamp comparator
bool isEarlier(const Data& a, const Data& b) {
    if (a.jam != b.jam) return a.jam < b.jam;
    return a.menit < b.menit;
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
        overJ.push_back({{"timestamp", (d.jam < 10 ? "0" : "") + to_string(d.jam) + ":" + (d.menit < 10 ? "0" : "") + to_string(d.menit)}, {"level", d.level}});
    }
    ofstream overFile("overflow.json");
    overFile << overJ.dump(4);
    overFile.close();

    //Save to underflow.json
    json underJ = json::array();
    for (const Data& d : waterlevel_dry) {
        underJ.push_back({{"timestamp", (d.jam < 10 ? "0" : "") + to_string(d.jam) + ":" + (d.menit < 10 ? "0" : "") + to_string(d.menit)}, {"level", d.level}});
    }
    ofstream underFile("underflow.json");
    underFile << underJ.dump(4);
    underFile.close();
}

// Masukkan ke JSON
void writeCritical(vector<Data> dataList) {
    json j;

    for (const Data& d : dataList) {
        if (d.level <= 25 || d.level >= 180) {
            json entry;
            entry["jam"] = d.jam;
            entry["menit"] = d.menit;
            entry["level"] = d.level;
            j.push_back(entry);
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

int main() {
    WSADATA wsa;
    SOCKET listen_socket, client_socket;
    struct sockaddr_in server, client;
    int c, recv_size;
    char messageRecv[100];
    string messageSend;

    WSAStartup(MAKEWORD(2,2), &wsa);

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
    listen(listen_socket, 3);

    cout << "Waiting for incoming connections..." << endl;

    c = sizeof(struct sockaddr_in);
    client_socket = accept(listen_socket, (struct sockaddr*)&client, &c);

    cout << "Connection accepted" << endl;


    // Menerima data dari client
    recv_size = recv(client_socket, messageRecv, sizeof(messageRecv) - 1, 0); 
    
    // Mengecek apakah terjadi kesalahan saat menerima data
    if (recv_size == SOCKET_ERROR) {
        cout << "Receive failed" << endl;
    } else {
        // Menambahkan null-terminator agar bisa diproses sebagai string C++
        messageRecv[recv_size] = '\0';  
        
        // Menampilkan pesan yang diterima dari client
        cout << "Received message from client: " << messageRecv << endl;
    }

    // Ambil data dan memasukkan data level kritikal ke JSON
    vector<Data> Data_fix = readFile(string messageRecv);
    writeCritical(Data_fix);
    sortAndSaveOverflowUnderflow(Data_fix);

    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();

    system("pause");
    return 0;
}
