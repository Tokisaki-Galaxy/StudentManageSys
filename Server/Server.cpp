// Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "Server.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sqlite3.h>

using namespace std;




Teacher::Teacher() {
        // 创建或打开SQLite3数据库
        int rc = sqlite3_open("students.db", &db);
        if (rc != SQLITE_OK) {
            cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        // 创建学生成绩表
        char* zErrMsg = 0;
        rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS students ("
            "id TEXT PRIMARY KEY,"
            "name TEXT,"
            "sex TEXT,"
            "age TEXT,"
            "grade1 REAL,"
            "grade2 REAL,"
            "grade3 REAL"
            ");", NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }

        // 从数据库读取学生成绩
        LoadData();
    }

Teacher::~Teacher() {
        // 关闭数据库连接
        sqlite3_close(db);
    }

void Teacher::Add() {
        string id, name, sex, age;
        float grade1, grade2, grade3;

        std::cout << "输入学号: ";
        std::cin >> id;
        std::cout << "输入姓名: ";
        std::cin >> name;
        std::cout << "输入性别: ";
        std::cin >> sex;
        std::cout << "输入年龄: ";
        std::cin >> age;
        std::cout << "输入实验成绩: ";
        std::cin >> grade1;
        std::cout << "输入考勤成绩: ";
        std::cin >> grade2;
        std::cout << "输入报告成绩: ";
        std::cin >> grade3;

        // 将新学生添加到数据库
        AddStudent(id, name, sex, age, grade1, grade2, grade3);
        system("pause");
    }

    void Teacher::Delete() {
        string id;
        std::cout << "输入要删除的学生ID: ";
        std::cin >> id;

        // 从数据库删除学生
        DeleteStudent(id);
        system("pause");
    }

    void Teacher::Modify(string id) {
        // 从数据库查找学生并修改
        ModifyStudent(id);
        system("pause");
    }

    void Teacher::Query() {
        // 从数据库查询学生信息
        QueryStudents();
        system("pause");
    }

    void Teacher::Save() {
        // 将所有学生信息保存到数据库
        SaveData();
        system("pause");
    }

    void Teacher::Load() {
        // 从数据库读取所有学生信息
        LoadData();
        system("pause");
    }

    void Teacher::DesTory() {
        // 清空数据库中的所有学生信息
        DestroyData();
        system("pause");
    }

    void Teacher::TJ() {
        // 统计学生成绩信息
        StatisticStudents();
        system("pause");
    }

    void Teacher::AddStudent(const string& id, const string& name, const string& sex, const string& age, float grade1, float grade2, float grade3) {
        // 将新学生添加到数据库
        char* zErrMsg = 0;
        string sql = "INSERT INTO students VALUES (?, ?, ?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, sex.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, age.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 5, grade1);
        sqlite3_bind_double(stmt, 6, grade2);
        sqlite3_bind_double(stmt, 7, grade3);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }

    void Teacher::DeleteStudent(const string& id) {
        // 从数据库删除学生
        char* zErrMsg = 0;
        string sql = "DELETE FROM students WHERE id = ?";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }

    void Teacher::ModifyStudent(const string& id) {
        // 从数据库查找学生并修改
        char* zErrMsg = 0;
        string sql = "SELECT * FROM students WHERE id = ?";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            string name = (char*)sqlite3_column_text(stmt, 1);
            string sex = (char*)sqlite3_column_text(stmt, 2);
            string age = (char*)sqlite3_column_text(stmt, 3);
            float grade1 = (float)sqlite3_column_double(stmt, 4);
            float grade2 = (float)sqlite3_column_double(stmt, 5);
            float grade3 = (float)sqlite3_column_double(stmt, 6);

            std::cout << "当前学生信息:\n"
                << "学号: " << id << "\n"
                << "姓名: " << name << "\n"
                << "性别: " << sex << "\n"
                << "年龄: " << age << "\n"
                << "实验成绩: " << grade1 << "\n"
                << "考勤成绩: " << grade2 << "\n"
                << "报告成绩: " << grade3 << "\n";

            std::cout << "请输入新的信息:\n";
            std::cin >> name >> sex >> age >> grade1 >> grade2 >> grade3;

            sql = "UPDATE students SET name = ?, sex = ?, age = ?, grade1 = ?, grade2 = ?, grade3 = ? WHERE id = ?";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
                return;
            }

            sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, sex.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, age.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 4, grade1);
            sqlite3_bind_double(stmt, 5, grade2);
            sqlite3_bind_double(stmt, 6, grade3);
            sqlite3_bind_text(stmt, 7, id.c_str(), -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            }

            sqlite3_finalize(stmt);
        }
        else {
            cerr << "未找到该学生" << endl;
        }

        sqlite3_finalize(stmt);
    }

    void Teacher::QueryStudents() {
        // 从数据库查询所有学生信息
        char* zErrMsg = 0;
        string sql = "SELECT * FROM students";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            string id = (char*)sqlite3_column_text(stmt, 0);
            string name = (char*)sqlite3_column_text(stmt, 1);
            string sex = (char*)sqlite3_column_text(stmt, 2);
            string age = (char*)sqlite3_column_text(stmt, 3);
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

        if (rc != SQLITE_DONE) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }

    void Teacher::SaveData() {
        // 将所有学生信息保存到数据库
        for (const auto& student : students) {
            AddStudent(student.Number, student.Name, student.Sex, student.Age, student.Grade1, student.Grade2, student.Grade3);
        }
    }

    void Teacher::LoadData() {
        // 从数据库读取所有学生信息
        char* zErrMsg = 0;
        string sql = "SELECT * FROM students";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        students.clear();
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            string id = (char*)sqlite3_column_text(stmt, 0);
            string name = (char*)sqlite3_column_text(stmt, 1);
            string sex = (char*)sqlite3_column_text(stmt, 2);
            string age = (char*)sqlite3_column_text(stmt, 3);
            float grade1 = (float)sqlite3_column_double(stmt, 4);
            float grade2 = (float)sqlite3_column_double(stmt, 5);
            float grade3 = (float)sqlite3_column_double(stmt, 6);

            students.emplace_back(id, name, sex, age, grade1, grade2, grade3);
        }

        if (rc != SQLITE_DONE) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }

    void Teacher::DestroyData() {
        // 清空数据库中的所有学生信息
        char* zErrMsg = 0;
        string sql = "DELETE FROM students";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }

    void Teacher::StatisticStudents() {
        // 从数据库读取学生成绩信息并统计
        char* zErrMsg = 0;
        string sql = "SELECT * FROM students";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
            return;
        }

        int totalStudents = 0, failGrade1 = 0, failGrade2 = 0, failGrade3 = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            totalStudents++;
            float grade1 = (float)sqlite3_column_double(stmt, 4);
            float grade2 = (float)sqlite3_column_double(stmt, 5);
            float grade3 = (float)sqlite3_column_double(stmt, 6);

            if (grade1 < 60) failGrade1++;
            if (grade2 < 60) failGrade2++;
            if (grade3 < 60) failGrade3++;
        }

        if (rc != SQLITE_DONE) {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);

        std::cout << "总共有 " << totalStudents << " 份成绩表\n"
            << "实验成绩不及格：" << failGrade1 << " 人\n"
            << "考勤成绩不及格：" << failGrade2 << " 人\n"
            << "报告成绩不及格：" << failGrade3 << " 人\n";
    }
    /*
int main() {
    Teacher m;
    int c;
    do {
        system("cls");
        std::cout << "                             欢迎使用学生成绩管理系统" << endl;
        std::cout << "                    ==========================================" << endl;
        std::cout << "                    ||  \t    1.新增学生成绩    \t   ||" << endl;
        std::cout << "                    ||  \t    2.删除学生成绩    \t   ||" << endl;
        std::cout << "                    ||  \t    3.修改学生成绩    \t   ||" << endl;
        std::cout << "                    ||  \t    4.查询详细信息  \t   ||" << endl;
        std::cout << "                    ||  \t    5.保存数据      \t   ||" << endl;
        std::cout << "                    ||  \t    6.清除学生成绩    \t   ||" << endl;
        std::cout << "                    ||  \t    7.统计名单    \t   ||" << endl;
        std::cout << "                    ||  \t    8.退出    \t\t   ||" << endl;
        std::cout << "                    ==========================================" << endl;
        std::cout << "   请选择(1-8):";
        std::cin >> c;
        switch (c)
        {
        case 1: m.Add(); break;
        case 2: m.Delete(); break;
        case 3: {
            string id;
            std::cout << "请输入要修改人员的ID:  ";
            std::cin >> id;
            m.Modify(id);
        }; break;
        case 4: m.Query(); break;
        case 5: m.Save(); break;
        case 6: m.DesTory(); break;
        case 7: m.TJ(); break;
        case 8: return 0;
        default: break;
        }
    } while (c != 8);

    return 0;
}*/