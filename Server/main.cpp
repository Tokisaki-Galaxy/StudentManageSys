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
        // 初始化 Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Failed to initialize Winsock." << endl;
            return;
        }

        // 创建服务器套接字
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            cerr << "Failed to create server socket: " << WSAGetLastError() << endl;
            WSACleanup();
            return;
        }

        // 绑定服务器地址和端口
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

        // 开始监听客户端连接
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            cerr << "Failed to listen on server socket: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        cout << "Server started. Listening on port 8888..." << endl;
    }

    ~Server() {
        // 关闭服务器套接字和 Winsock
        closesocket(serverSocket);
        WSACleanup();
    }

    void run() {
        while (true) {
            // 等待客户端连接
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "Failed to accept client connection: " << WSAGetLastError() << endl;
                continue;
            }

            cout << "New client connected." << endl;

            // 处理客户端请求
            handleClient(clientSocket);

            // 关闭客户端套接字
            closesocket(clientSocket);
        }
    }

private:
    void handleClient(SOCKET clientSocket) {
        char buffer[1024];
        int bytesReceived;
        while (true) {
            // 接收客户端发送的命令
            ZeroMemory(buffer, sizeof(buffer));
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived == SOCKET_ERROR) {
                cerr << "Error receiving data from client: " << WSAGetLastError() << endl;
                break;
            }
            if (bytesReceived == 0) {
                // 客户端断开连接
                cout << "Client disconnected." << endl;
                break;
            }

            // 处理客户端命令
            string command(buffer, bytesReceived);
            handleCommand(clientSocket, command);
        }
    }

    void handleCommand(SOCKET clientSocket, const string& command) {
        // 根据客户端命令执行相应操作
        if (command == "1") {
            teacher.Add();
            sendResponse(clientSocket, "成功添加新学生成绩");
        }
        else if (command == "2") {
            teacher.Delete();
            sendResponse(clientSocket, "成功删除学生成绩");
        }
        else if (command.substr(0, 2) == "3 ") {
            string id = command.substr(2);
            teacher.Modify(id);
            sendResponse(clientSocket, "成功修改学生成绩");
        }
        else if (command == "4") {
            string response = queryStudents();
            sendResponse(clientSocket, response);
        }
        else if (command == "5") {
            teacher.Save();
            sendResponse(clientSocket, "成功保存数据");
        }
        else if (command == "6") {
            teacher.DesTory();
            sendResponse(clientSocket, "成功清除学生成绩");
        }
        else if (command == "7") {
            string response = statisticStudents();
            sendResponse(clientSocket, response);
        }
        else {
            sendResponse(clientSocket, "无效命令");
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
            response += "学号: " + student.Number + "\n"
                "姓名: " + student.Name + "\n"
                "性别: " + student.Sex + "\n"
                "年龄: " + student.Age + "\n"
                "实验成绩: " + to_string(student.Grade1) + "\n"
                "考勤成绩: " + to_string(student.Grade2) + "\n"
                "报告成绩: " + to_string(student.Grade3) + "\n\n";
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

        string response = "总共有 " + to_string(totalStudents) + " 份成绩表\n"
            "实验成绩不及格：" + to_string(failGrade1) + " 人\n"
            "考勤成绩不及格：" + to_string(failGrade2) + " 人\n"
            "报告成绩不及格：" + to_string(failGrade3) + " 人\n";
        return response;
    }
};

int main() {
    Server server;
    server.run();
    return 0;
}