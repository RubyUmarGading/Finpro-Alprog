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

// Masukkan ke JSON
void writeCritical(vector<Data> dataList) {
    json j;

    for (const Data& d : dataList) {
        if (d.level <= 10 || d.level >= 200) {
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

    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();

    system("pause");
    return 0;
}
