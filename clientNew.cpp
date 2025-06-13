#include <iostream>
#include <winsock2.h>
using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: clientNew <level>\n";
        return 1;
    }

    const char* level = argv[1];

    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    WSAStartup(MAKEWORD(2,2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cerr << "Connection failed.\n";
        return 1;
    }

    send(s, level, strlen(level), 0);
    cout << "Sent level: " << level << endl;

    closesocket(s);
    WSACleanup();
    return 0;
}
