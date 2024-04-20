// StudentManageSys.cpp
//

#include "StudentManageSys.h"


function Client::ConnectServer() -> int
{
	return 0;
}

Client::Client()
{
}

Client::~Client()
{
}

function Client::GetCommand() -> std::string
{
	std::string command;
	std::cout << "Please enter a command: ";
	std::getline(std::cin, command);
	return command;
}