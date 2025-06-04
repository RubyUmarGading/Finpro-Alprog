#include <iostream>
#include <winsock2.h>
#include <string>
#include "AyamAyam.hpp"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8888

int main() {
    WSADATA wsa;
    SOCKET listen_socket, client_socket;
    struct sockaddr_in server, client;
    int c, recv_size;
    char messageRecv[100];

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
    if (recv_size == SOCKET_ERROR) {
        cout << "Receive failed" << endl;
    } else {
        messageRecv[recv_size] = '\0';
        cout << "Received message from client: " << messageRecv << endl;
    }

    // Process the file and write outputs
    vector<Data> Data_fix = readFile(string(messageRecv));
    writeCritical(Data_fix);
    sortAndSaveOverflowUnderflow(Data_fix);

    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();

    return 0;
}
