// Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "Server.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sqlite3.h>

using namespace std;

Teacher::Teacher()
{
    // 检查是否存在数据库文件
    // 如果不存在则创建数据库文件
    if (std::filesystem::exists("students.db") == false)
    {
        cerr << "数据库文件不存在，正在创建数据库文件" << endl;
    }
    // 创建或打开SQLite3数据库
    int rc = sqlite3_open("students.db", &db);
    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // 创建学生成绩表
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS students ("
                          "id TEXT PRIMARY KEY,"
                          "name TEXT,"
                          "sex TEXT,"
                          "age TEXT,"
                          "grade1 REAL,"
                          "grade2 REAL,"
                          "grade3 REAL"
                          ");",
                      NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }

    // 从数据库读取学生成绩
    LoadData();
}

Teacher::~Teacher()
{
    // 关闭数据库连接
    sqlite3_close(db);
}

void Teacher::AddStudent(std::string_view id, std::string_view name, std::string_view sex, std::string_view age, float grade1, float grade2, float grade3)
{
    // 将新学生添加到数据库
    string sql = "INSERT INTO students VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, id.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, sex.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, age.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, grade1);
    sqlite3_bind_double(stmt, 6, grade2);
    sqlite3_bind_double(stmt, 7, grade3);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void Teacher::DeleteStudent(std::string_view id)
{
    // 从数据库删除学生
    string sql = "DELETE FROM students WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, id.data(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void Teacher::ModifyStudent(std::string_view id, std::string_view aid, std::string_view name, std::string_view sex, std::string_view age, float grade1, float grade2, float grade3)
{
    string sql = "UPDATE students SET id = ?, name = ?, sex = ?, age = ?, grade1 = ?, grade2 = ?, grade3 = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, aid.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, sex.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, age.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, grade1);
    sqlite3_bind_double(stmt, 6, grade2);
    sqlite3_bind_double(stmt, 7, grade3);
    sqlite3_bind_text(stmt, 8, id.data(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    LoadData(); // 重新加载数据
}

void Teacher::QueryStudents()
{
    // 从数据库查询所有学生信息
    string sql = "SELECT * FROM students";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        string id = (char *)sqlite3_column_text(stmt, 0);
        string name = (char *)sqlite3_column_text(stmt, 1);
        string sex = (char *)sqlite3_column_text(stmt, 2);
        string age = (char *)sqlite3_column_text(stmt, 3);
        float grade1 = (float)sqlite3_column_double(stmt, 4);
        float grade2 = (float)sqlite3_column_double(stmt, 5);
        float grade3 = (float)sqlite3_column_double(stmt, 6);

        std::cout << "学号: " << id << "\n"
                  << "姓名: " << name << "\n"
                  << "性别: " << sex << "\n"
                  << "年龄: " << age << "\n"
                  << "实验成绩: " << grade1 << "\n"
                  << "考勤成绩: " << grade2 << "\n"
                  << "报告成绩: " << grade3 << "\n\n";
    }

    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void Teacher::SaveData()
{
    for (const auto& student : students)
    {
        string sql = "UPDATE students SET name = ?, sex = ?, age = ?, grade1 = ?, grade2 = ?, grade3 = ? WHERE id = ?";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
        if (rc != SQLITE_OK)
        {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            continue;
        }

        sqlite3_bind_text(stmt, 1, student.Name.data(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, student.Sex.data(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, student.Age.data(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, student.Grade1);
        sqlite3_bind_double(stmt, 5, student.Grade2);
        sqlite3_bind_double(stmt, 6, student.Grade3);
        sqlite3_bind_text(stmt, 7, student.Number.data(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }
}

void Teacher::LoadData()
{
    // 从数据库读取所有学生信息
    string sql = "SELECT * FROM students";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    students.clear();
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        string id = (char *)sqlite3_column_text(stmt, 0);
        string name = (char *)sqlite3_column_text(stmt, 1);
        string sex = (char *)sqlite3_column_text(stmt, 2);
        string age = (char *)sqlite3_column_text(stmt, 3);
        float grade1 = (float)sqlite3_column_double(stmt, 4);
        float grade2 = (float)sqlite3_column_double(stmt, 5);
        float grade3 = (float)sqlite3_column_double(stmt, 6);

        students.emplace_back(id, name, sex, age, grade1, grade2, grade3);
    }

    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void Teacher::DestroyData()
{
    // 清空数据库中的所有学生信息
    string sql = "DELETE FROM students";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void Teacher::StatisticStudents()
{
    // 从数据库读取学生成绩信息并统计
    string sql = "SELECT * FROM students";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    int totalStudents = 0, failGrade1 = 0, failGrade2 = 0, failGrade3 = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        totalStudents++;
        float grade1 = (float)sqlite3_column_double(stmt, 4);
        float grade2 = (float)sqlite3_column_double(stmt, 5);
        float grade3 = (float)sqlite3_column_double(stmt, 6);

        if (grade1 < 60)
            failGrade1++;
        if (grade2 < 60)
            failGrade2++;
        if (grade3 < 60)
            failGrade3++;
    }

    if (rc != SQLITE_DONE)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);

    std::cout << "总共有 " << totalStudents << " 份成绩表\n"
              << "实验成绩不及格：" << failGrade1 << " 人\n"
              << "考勤成绩不及格：" << failGrade2 << " 人\n"
              << "报告成绩不及格：" << failGrade3 << " 人\n";
}

Server::Server() : teacher()
{
    // 初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "Failed to initialize Winsock." << endl;
        return;
    }

    // 创建服务器套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cerr << "Failed to create server socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // 绑定服务器地址和端口
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Failed to bind server socket: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // 开始监听客户端连接
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "Failed to listen on server socket: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    cout << "Server started. Listening on port 8888..." << endl;
}

Server::~Server()
{
    // 关闭服务器套接字和 Winsock
    closesocket(serverSocket);
    WSACleanup();
}

function Server::run() -> void
{
    while (true)
    {
        // 等待客户端连接
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            cerr << "Failed to accept client connection: " << WSAGetLastError() << endl;
            Sleep(3000);
            continue;
        }

        cout << "New client connected." << endl;

        // 处理客户端请求
        handleClient(clientSocket);

        // 关闭客户端套接字
        closesocket(clientSocket);
    }
    return;
}

function Server::handleClient(SOCKET clientSocket) -> void
{
    while (true)
    {
        teacher.LoadData();
        menu(clientSocket);
        // 接收客户端发送的命令
        std::string command = reciveCommand(clientSocket);

        // 检查是否接收到错误或异常
        if (command == "error" || command.empty()) // 你需要根据 reciveCommand 的实现来检查错误
        {
            cout << "Client disconnected." << endl;
            break;
        }

        // 处理客户端命令
        handleCommand(clientSocket, command);
        sendResponse(clientSocket, "client-console-cls");
        Sleep(300);
    }
    return;
}

function Server::handleCommand(SOCKET clientSocket, std::string_view command) -> void
{
    // 根据客户端命令执行相应操作
    switch (stoi(command.data()))
    {
    case 1:
    {
        sendResponse(clientSocket, "请输入你要添加的学生学号（ID）");
        std::string id = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入你要添加的学生姓名");
        std::string name = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入你要添加的学生性别");
        std::string sex = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入你要添加的学生年龄");
        std::string age = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入你要添加的学生实验成绩");
        float grage1 = std::stof(reciveCommand(clientSocket));
        sendResponse(clientSocket, "请输入你要添加的学生考勤成绩");
        float grage2 = std::stof(reciveCommand(clientSocket));
        sendResponse(clientSocket, "请输入你要添加的学生报告成绩");
        float grage3 = std::stof(reciveCommand(clientSocket));
        teacher.AddStudent(id, name, sex, age, grage1, grage2, grage3);
        sendResponse(clientSocket, "成功添加新学生成绩");
        break;
    }
    case 2:
        sendResponse(clientSocket, "请输入你要删除的学生的学号");
        teacher.DeleteStudent(reciveCommand(clientSocket));
        sendResponse(clientSocket, "成功删除学生成绩");
        break;
    case 3:
    {
        sendResponse(clientSocket, "请输入你要修改的学生的学号");
        std::string id = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入修改后学生学号（ID）");
        std::string aid = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入修改后学生姓名");
        std::string name = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入修改后学生性别");
        std::string sex = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入修改后学生年龄");
        std::string age = reciveCommand(clientSocket);
        sendResponse(clientSocket, "请输入修改后学生实验成绩");
        float grade1 = std::stof(reciveCommand(clientSocket));
        sendResponse(clientSocket, "请输入修改后学生考勤成绩");
        float grade2 = std::stof(reciveCommand(clientSocket));
        sendResponse(clientSocket, "请输入修改后学生报告成绩");
        float grade3 = std::stof(reciveCommand(clientSocket));
        teacher.ModifyStudent(id, aid, name,sex,age,grade1,grade2,grade3);
        sendResponse(clientSocket, "成功修改学生成绩");
        break;
    }
    case 4:
        sendResponse(clientSocket, queryStudents());
        break;
    case 5:
        teacher.SaveData();
        sendResponse(clientSocket, "成功保存数据");
        break;
    case 6:
        teacher.DestroyData();
        sendResponse(clientSocket, "成功清除学生成绩");
        break;
    case 7:
        sendResponse(clientSocket, statisticStudents());
        break;
    default:
        sendResponse(clientSocket, "无效命令");
        break;
    }
    teacher.SaveData();
    return;
}

function Server::sendResponse(SOCKET clientSocket, std::string_view response) -> void
{
    if (send(clientSocket, response.data(), static_cast<int>(response.length()), 0) == SOCKET_ERROR)
    {
        cerr << "Error sending data to client: " << WSAGetLastError() << endl;
    }
}

function Server::reciveCommand(SOCKET clientSocket) -> std::string
{
    std::vector<char> buffer(1024);
    int bytesReceived = recv(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        std::cerr << "Error receiving data from client: " << WSAGetLastError() << std::endl;
        return "";
    }
    else if (bytesReceived == 0)
    {
        // 客户端断开连接
        std::cout << "Client disconnected." << std::endl;
        return "";
    }
    return std::string(buffer.begin(), buffer.begin() + bytesReceived);
}

function Server::queryStudents() -> std::string
{
    std::string response;
    for (const auto &student : this->teacher.students)
    {
        response += "学号: " + student.Number + "\n"
                                                "姓名: " +
                    student.Name + "\n"
                                   "性别: " +
                    student.Sex + "\n"
                                  "年龄: " +
                    student.Age + "\n"
                                  "实验成绩: " +
                    to_string(student.Grade1) + "\n"
                                                "考勤成绩: " +
                    to_string(student.Grade2) + "\n"
                                                "报告成绩: " +
                    to_string(student.Grade3) + "\n\n";
    }
    return response;
}

function Server::statisticStudents() -> std::string
{
    int totalStudents = 0, failGrade1 = 0, failGrade2 = 0, failGrade3 = 0;
    for (const auto &student : this->teacher.students)
    {
        totalStudents++;
        if (student.Grade1 < 60)
            failGrade1++;
        if (student.Grade2 < 60)
            failGrade2++;
        if (student.Grade3 < 60)
            failGrade3++;
    }

    string response = "总共有 " + to_string(totalStudents) + " 份成绩表\n"
                                                             "实验成绩不及格：" +
                      to_string(failGrade1) + " 人\n"
                                              "考勤成绩不及格：" +
                      to_string(failGrade2) + " 人\n"
                                              "报告成绩不及格：" +
                      to_string(failGrade3) + " 人\n";
    return response;
}


function Server::menu(SOCKET clientSocket)->int {
    string response= "\n                             欢迎使用学生成绩管理系统\n";
    response += "                    ==========================================\n";
    response += "                    ||  \t    1.新增学生成绩    \t   ||\n";
    response += "                    ||  \t    2.删除学生成绩    \t   ||\n";
    response += "                    ||  \t    3.修改学生成绩    \t   ||\n";
    response += "                    ||  \t    4.查询详细信息  \t   ||\n";
    response += "                    ||  \t    5.保存数据      \t   ||\n";
    response += "                    ||  \t    6.清除学生成绩    \t   ||\n";
    response += "                    ||  \t    7.统计名单    \t   ||\n";
    response += "                    ==========================================\n";
    response += "   请选择(1-8):";
    sendResponse(clientSocket, response);
    return 0;
}