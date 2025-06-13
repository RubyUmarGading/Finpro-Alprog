#include <iostream>
#include <string>
#include <winsock2.h>
#include <fstream>
#include <vector>
#include "json.hpp"
#include "AyamAyam.hpp"

using namespace std;
using json = nlohmann::json;

#define PORT 8888

int main() {
    WSADATA wsa;
    SOCKET listen_socket, client_socket;
    struct sockaddr_in server, client;
    int c, recv_size;
    char buffer[100];

    WSAStartup(MAKEWORD(2,2), &wsa);

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
    listen(listen_socket, 3);

    cout << "Waiting for incoming connections..." << endl;

    // Menerima data dari water tank simulator
    while (true) {
        c = sizeof(struct sockaddr_in);
        client_socket = accept(listen_socket, (struct sockaddr*)&client, &c);

        cout << "Connection accepted" << endl;

        if (client_socket == INVALID_SOCKET) continue;

        cout << "Client connected\n";

        recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_size != SOCKET_ERROR) {
            buffer[recv_size] = '\0';
            cout << "Received water level: " << buffer << endl;

            // Waktu server
            SYSTEMTIME st;
            GetLocalTime(&st);
            int level = atoi(buffer);

            char timestamp[16];
            sprintf(timestamp, "%02d:%02d:%02d-%d", st.wHour, st.wMinute, st.wSecond, level);

            // Masukkan ke log
            ofstream fout("server_log.txt", ios::app);
            fout << timestamp << endl;
            fout.close();

            // Masukkan nilai critical ke JSON
            vector<Data> logs = readLogFile("server_log.txt");
            writeCritical(logs, "critical_log.json");

            // Sorting overflow dan dry run
            sortAndSaveOverflowUnderflow(logs);
            
        }

        closesocket(client_socket); 
    }

    
    closesocket(listen_socket);
    WSACleanup();

    system("pause");
    return 0;
}
