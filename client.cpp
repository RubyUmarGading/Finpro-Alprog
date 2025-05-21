#include <iostream>
#include <string>
#include <winsock2.h>
using namespace std;

#define SERVER "127.0.0.1"
#define PORT 8888

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    string file;

    // Inisialisasi Winsock dengan pengecekan error 
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        cout << "WSAStartup failed" << endl;
        return 1;
    }

    // Membuat socket dengan pengecekan error
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        cout << "Could not create socket" << endl;
        WSACleanup();
        return 1;
    }

    // Konfigurasi alamat server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER);
    server.sin_port = htons(PORT);

    // Menghubungkan ke server dengan pengecekan error 
    cout << "Connecting to server..." << endl;
    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cout << "Connection failed: " << WSAGetLastError() << endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server!" << endl;

    //  Menggunakan getline agar bisa input kalimat lengkap dengan spasi 
    cout << "Kirim data ke server: ";
    getline(cin, file); // Mengganti cin file agar bisa menangkap input dengan spasi

    // Mengirim data ke server
    if (send(client_socket, file.c_str(), file.length(), 0) == SOCKET_ERROR) {
        // Pengecekan error saat pengiriman ===
        cout << "Send failed: " << WSAGetLastError() << endl;
    }

    // Menutup koneksi dan membersihkan Winsock
    closesocket(client_socket);
    WSACleanup();

    system("pause");
    return 0;
}
