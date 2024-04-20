// StudentManageSys.h
//

#pragma once
#include <asio.hpp>
#include <string>
#include <map>
#include <iostream>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <string_view>

#define function auto

class Client {
	std::string ip;
	std::string port;
public:
	Client();
	~Client();
	function EncryptData(std::string_view data,std::string_view enkey)->std::string;
	function DecryptData(std::string_view data,std::string_view enkey)->std::string;
	function ConnectServer()->int;
	function SendCommand()->int;
	function GetCommand()->std::string;
};