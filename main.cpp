#include <iostream>
#include <fstream>
#include <memory>

#include <OTA.h>
#include <frame.h>
#include <msg_manager.h>


int main() {
    OTA_init("/dev/tty24", B115200);
    std::ifstream file("/home/yunxiao/Desktop/Sensorhub_UART/firmware/APP.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    constexpr std::streamsize chunkSize = 65536;
    auto buffer = std::make_unique<char[]>(chunkSize); // C++14 起支持

    try {
        const auto bytesRead = read_bin(file, buffer.get(), chunkSize);
        std::cout << "读取 " << bytesRead << " 字节" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}