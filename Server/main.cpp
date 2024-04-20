#include "Server.h"
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Server {
private:
    SOCKET serverSocket;
    SOCKET outputSocket;
    Teacher teacher;

public:
    Server() : teacher() {
        // ��ʼ�� Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Failed to initialize Winsock." << endl;
            return;
        }

        // �����������׽���
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            cerr << "Failed to create server socket: " << WSAGetLastError() << endl;
            WSACleanup();
            return;
        }

        // �󶨷�������ַ�Ͷ˿�
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(8888);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Failed to bind server socket: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        // ��ʼ�����ͻ�������
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            cerr << "Failed to listen on server socket: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        cout << "Server started. Listening on port 8888..." << endl;
    }

    ~Server() {
        // �رշ������׽��ֺ� Winsock
        closesocket(serverSocket);
        WSACleanup();
    }

    void run() {
        while (true) {
            // �ȴ��ͻ�������
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "Failed to accept client connection: " << WSAGetLastError() << endl;
                continue;
            }

            cout << "New client connected." << endl;

            // ����ͻ�������
            handleClient(clientSocket);

            // �رտͻ����׽���
            closesocket(clientSocket);
        }
    }

private:
    void handleClient(SOCKET clientSocket) {
        char buffer[1024];
        int bytesReceived;
        while (true) {
            // ���տͻ��˷��͵�����
            ZeroMemory(buffer, sizeof(buffer));
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived == SOCKET_ERROR) {
                cerr << "Error receiving data from client: " << WSAGetLastError() << endl;
                break;
            }
            if (bytesReceived == 0) {
                // �ͻ��˶Ͽ�����
                cout << "Client disconnected." << endl;
                break;
            }

            // ����ͻ�������
            string command(buffer, bytesReceived);
            handleCommand(clientSocket, command);
        }
    }

    void handleCommand(SOCKET clientSocket, const string& command) {
        // ���ݿͻ�������ִ����Ӧ����
        if (command == "1") {
            teacher.Add();
            sendResponse(clientSocket, "�ɹ������ѧ���ɼ�");
        }
        else if (command == "2") {
            teacher.Delete();
            sendResponse(clientSocket, "�ɹ�ɾ��ѧ���ɼ�");
        }
        else if (command.substr(0, 2) == "3 ") {
            string id = command.substr(2);
            teacher.Modify(id);
            sendResponse(clientSocket, "�ɹ��޸�ѧ���ɼ�");
        }
        else if (command == "4") {
            string response = queryStudents();
            sendResponse(clientSocket, response);
        }
        else if (command == "5") {
            teacher.Save();
            sendResponse(clientSocket, "�ɹ���������");
        }
        else if (command == "6") {
            teacher.DesTory();
            sendResponse(clientSocket, "�ɹ����ѧ���ɼ�");
        }
        else if (command == "7") {
            string response = statisticStudents();
            sendResponse(clientSocket, response);
        }
        else {
            sendResponse(clientSocket, "��Ч����");
        }
    }

    void sendResponse(SOCKET clientSocket, const string& response) {
        int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            cerr << "Error sending data to client: " << WSAGetLastError() << endl;
        }
    }

    string queryStudents() {
        string response;
        for (const auto& student : this->teacher.students) {
            response += "ѧ��: " + student.Number + "\n"
                "����: " + student.Name + "\n"
                "�Ա�: " + student.Sex + "\n"
                "����: " + student.Age + "\n"
                "ʵ��ɼ�: " + to_string(student.Grade1) + "\n"
                "���ڳɼ�: " + to_string(student.Grade2) + "\n"
                "����ɼ�: " + to_string(student.Grade3) + "\n\n";
        }
        return response;
    }

    string statisticStudents() {
        int totalStudents = 0, failGrade1 = 0, failGrade2 = 0, failGrade3 = 0;
        for (const auto& student : this->teacher.students) {
            totalStudents++;
            if (student.Grade1 < 60) failGrade1++;
            if (student.Grade2 < 60) failGrade2++;
            if (student.Grade3 < 60) failGrade3++;
        }

        string response = "�ܹ��� " + to_string(totalStudents) + " �ݳɼ���\n"
            "ʵ��ɼ�������" + to_string(failGrade1) + " ��\n"
            "���ڳɼ�������" + to_string(failGrade2) + " ��\n"
            "����ɼ�������" + to_string(failGrade3) + " ��\n";
        return response;
    }
};

int main() {
    Server server;
    server.run();
    return 0;
}