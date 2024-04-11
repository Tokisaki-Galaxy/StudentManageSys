// Client.cpp
//

#include "StudentManageSys.h"

auto main() -> int {
    std::cout << "Hello World!\n";
    Client Sy("admin", "admin");
    if (!Sy.login()){
        MessageBox(NULL, L"Login failed!", L"Error", MB_OK);
        exit(-1);
    }

    return 0;
}