// Client.cpp
//

#include "StudentManageSys.h"// Client.cpp


#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed to create client socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr)); // 使用 inet_pton 替换 inet_addr
    serverAddr.sin_port = htons(8888); // 服务器端口号

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Failed to connect to server: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server." << endl;

    thread receiveThread([&]() {
        char buffer[1024];
        while (true) {
            ZeroMemory(buffer, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived == SOCKET_ERROR) {
                cerr << "Error receiving data from server: " << WSAGetLastError() << endl;
                break;
            }
            if (bytesReceived == 0) {
                // 服务器断开连接
                cout << "Server disconnected." << endl;
                break;
            }
            cout << "Received: " << buffer << endl;
        }
        closesocket(clientSocket);
        WSACleanup();
        });
    receiveThread.detach();

    string message;
    while (true) {
        getline(cin, message);
        if (message == "exit") {
            break;
        }
        int bytesSent = send(clientSocket, message.c_str(), message.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            cerr << "Error sending data to server: " << WSAGetLastError() << endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}