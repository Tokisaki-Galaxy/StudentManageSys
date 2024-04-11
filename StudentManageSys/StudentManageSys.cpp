// StudentManageSys.cpp
//

#include "StudentManageSys.h"

bool Client::login(std::string_view username, std::string_view password)
{
    // Connect to the server
    asio::ip::tcp::socket socket(asio::detail::strand_service::io_context_);
    socket.connect(server_endpoint_);

    // Send the username and password to the server
    asio::write(socket, asio::buffer(username.data(), username.size()));
    asio::write(socket, asio::buffer("\n"));
    asio::write(socket, asio::buffer(password.data(), password.size()));
    asio::write(socket, asio::buffer("\n"));

    // Read the response from the server
    asio::streambuf buf;
    asio::read_until(socket, buf, '\n');
    std::string response = asio::buffer_cast<const char*>(buf.data());

    // If the response is "Login successful\n", return true
    // Otherwise, return false
    return response == "Login successful\n";
}

std::map<std::string, std::string> Client::get_student_info()
{
	return std::map<std::string, std::string>();
}

std::string Client::encrypt_password(const std::string& password)
{
	return std::string();
}
