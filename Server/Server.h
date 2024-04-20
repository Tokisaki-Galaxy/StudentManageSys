// Server.h
//

#pragma once
#include <map>
#include <string>
#include <vector>
#include <optional>
#include <WinSock2.h>
#include <Windows.h>
#include <sqlite3.h>

#define function auto
using namespace std;
/*
class Server
{
private:
    SOCKET serverSocket;
    SOCKET outputSocket;
    Teacher teacher;
public:
    Server() : teacher();
    ~Server() { SetConsoleCtrlHandler(CtrlHandler, FALSE); };
    inline static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
    {
        switch (fdwCtrlType)
        {
        case CTRL_CLOSE_EVENT:
            // 在这里执行需要在关闭时进行的清理操作
            return TRUE;
        default:
            return FALSE;
        }
    }

    function InitSSL()->int;
    function HandleRequest()->int;
    function SetEnv()->std::string;
};*/

class Student
{
public:
    std::string Number;
    std::string Name;
    std::string Sex;
    std::string Age;
    float Grade1;
    float Grade2;
    float Grade3;

    Student(string id, string name, string sex, string age, float grade1, float grade2, float grade3)
        : Number(id), Name(name), Sex(sex), Age(age), Grade1(grade1), Grade2(grade2), Grade3(grade3) {}
};

class Teacher {
public:
    vector<Student> students;
    sqlite3* db;

public:
    Teacher();
    ~Teacher();
    void Add();
    void Delete();
    void Modify(std::string id);
    void Query();
    void Save();
    void Load();
    void DesTory();
    void TJ();

private:
    void AddStudent(const std::string &id, const std::string &name, const std::string &sex, const std::string &age, float grade1, float grade2, float grade3);
    void DeleteStudent(const std::string &id);
    void ModifyStudent(const std::string &id);
    void QueryStudents();
    void SaveData();
    void LoadData();
    void DestroyData();
    void StatisticStudents();
};


class Server
{
private:
    SOCKET serverSocket;
    SOCKET outputSocket;
    Teacher teacher;

public:
    Server();
    ~Server();
    void run();

private:
    void handleClient(SOCKET clientSocket);
    void handleCommand(SOCKET clientSocket, const string& command);
    void sendResponse(SOCKET clientSocket, const string& response);
    string queryStudents();
    string statisticStudents();
};