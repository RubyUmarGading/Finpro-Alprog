#include <iostream>
#include <string>
#include <winsock2.h>
#include <bits/stdc++.h>
#include <fstream>
#include <vector>
using namespace std;

#define PORT 8888

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


    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();

    system("pause");
    return 0;
}
