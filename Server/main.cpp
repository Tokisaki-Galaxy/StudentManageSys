#include "Server.h"
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    Server server;
    server.run();
    return 0;
}