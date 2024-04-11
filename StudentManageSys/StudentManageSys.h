// StudentManageSys.h
//
#pragma once
#include <asio.hpp>
#include <string>
#include <map>
#include <iostream>

class Client {
public:
    Client(asio::io_context& io_context, const std::string& server, const std::string& port)
        : socket_(io_context) {
        // 连接到服务器
        asio::ip::tcp::resolver resolver(io_context);
        asio::connect(socket_, resolver.resolve(server, port));
    }
    ~Client();

    bool login(std::string_view username, std::string_view password);
    std::map<std::string, std::string> get_student_info();

private:
    std::string encrypt_password(const std::string& password);

    asio::ip::tcp::socket socket_;
};