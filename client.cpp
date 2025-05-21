#include <iostream>
#include <string>
#include <winsock2.h>
#include <sstream>

using namespace std;

#define SERVER "127.0.0.1"
#define PORT 8888

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    int recv_size;
    string messageSend;

    WSAStartup(MAKEWORD(2,2), &wsa);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER);
    server.sin_port = htons(PORT);

    connect(client_socket, (struct sockaddr*)&server, sizeof(server));

    string file;

	cout << "Kirim data ke server ";
	cin >> file; 

	messageSend = file;

    send(client_socket, messageSend.c_str(), messageSend.length(), 0);

    closesocket(client_socket);
    WSACleanup();

	system("pause");
    return 0;
}