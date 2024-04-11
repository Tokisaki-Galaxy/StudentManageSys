#pragma once

#include <map>
#include <string>
#include <vector>
#include <optional>

// 使用C++23的模块和概念
export module server;

// 学生信息
struct Student {
    std::string name;
    int id;
    std::map<std::string, int> scores; // 课程和成绩
};

class Server {
public:
    // 登录
	bool login(const std::string& username, const std::string& password);
	// 注销
	void logout(const std::string& username);
	// 获取用户权限
	std::string getPermission(const std::string& username) const;
	// 设置用户权限
	void setPermission(const std::string& username, const std::string& permission);
private:
    std::string username;
    std::string permission;
};

// 用户数据库
class UserDatabase {
public:
	// 添加用户
	void addUser(const std::string& username, const std::string& password);
	// 删除用户
	void removeUser(const std::string& username);
	// 更新用户密码
	void updatePassword(const std::string& username, const std::string& password);
	// 查询所有用户列表
	std::vector<std::string> getAllUsers() const;
private:
    std::map<std::string, std::string> users_; // 用户名
    std::map<std::string, std::string> permissions_; // 用户名和权限
};

// 学生数据库
class StudentDatabase {
public:
    // 添加学生
    void addStudent(const Student& student);
    // 删除学生
    void removeStudent(int id);
    // 更新学生信息
    void updateStudent(const Student& student);
    // 查询学生信息
    std::optional<Student> getStudent(int id) const;
    // 查询所有学生信息
    std::vector<Student> getAllStudents() const;
private:
    std::map<int, Student> students_; // 学生ID和学生信息
};
