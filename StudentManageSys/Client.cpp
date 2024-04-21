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

    string serverIP;
    string serverPort;
    std::cout << "输入服务器IP地址(为空则默认127.0.0.1):";
    getline(cin, serverIP);
    if (serverIP.empty()) {
        serverIP = "127.0.0.1";
    }

    std::cout << "输入服务器端口(为空则默认8888):";
    getline(cin, serverPort);
    if (serverPort.empty()) {
        serverPort = "8888";
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.data(), &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(stoi(serverPort));

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
            std::string receivedMessage(buffer, bytesReceived);
            if (receivedMessage == "client-console-cls") {
				//system("cls");
                cout<< endl<<endl << endl;
			}
			else{
            cout << "Received: \n" << buffer << endl;
            }
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
        int bytesSent = send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);
        if (bytesSent == SOCKET_ERROR) {
            cerr << "Error sending data to server: " << WSAGetLastError() << endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}