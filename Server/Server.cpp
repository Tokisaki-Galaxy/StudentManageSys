// Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "Server.h"

auto main() -> int {
    try {
        asio::io_context io_context;

        Server s(io_context, 3001);

        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
