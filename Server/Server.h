// Server.h
//

#pragma once
#include <map>
#include <string>
#include <vector>
#include <optional>
#include <WinSock2.h>
#include <sqlite3.h>

#define function auto

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

    Student(std::string id, std::string name, std::string sex, std::string age, float grade1, float grade2, float grade3)
        : Number(id), Name(name), Sex(sex), Age(age), Grade1(grade1), Grade2(grade2), Grade3(grade3) {}
};

class Teacher {
public:
    std::vector<Student> students;
    sqlite3* db;

public:
    Teacher();
    ~Teacher();

    void AddStudent(std::string_view id, std::string_view name, std::string_view sex, std::string_view age, float grade1, float grade2, float grade3);
    void DeleteStudent(std::string_view id);
    void ModifyStudent(std::string_view id, std::string_view aid, std::string_view name, std::string_view sex, std::string_view age, float grade1, float grade2, float grade3);
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
    Teacher teacher;

public:
    Server();
    ~Server();
    function run() -> void;

private:
    function handleClient(SOCKET clientSocket)                               ->void;
    function handleCommand(SOCKET clientSocket, std::string_view command)    ->void;
    function sendResponse(SOCKET clientSocket, std::string_view response)    ->void;
    function reciveCommand(SOCKET clientSocket)                              ->std::string;
    function queryStudents()                                                 ->std::string;
    function statisticStudents()                                             ->std::string;
    function SetEnv()                                                        ->std::string;
    function menu(SOCKET clientSocket)                                       ->int;
};