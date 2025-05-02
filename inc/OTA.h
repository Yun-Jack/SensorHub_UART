#include <msg_manager.h>

#pragma once
#include <cstdint>
#include <fstream>
#include <termios.h>

// 前置声明减少头文件依赖
class MsgManager;

// 使用 pragma pack 确保结构体紧凑（按1字节对齐）
#pragma pack(push, 1)
struct OTA_Data {
    uint16_t bin_length;
    char* bin_data; // 改为动态指针，支持可变长度数据

    // 命令字符串使用 constexpr 定义，避免重复初始化
    static constexpr const char* update_cmd = "#update\r\n";
    static constexpr const char* reboot_cmd = "#reboot\r\n";
};
#pragma pack(pop)

// 函数声明
bool OTA_init(const char* device, speed_t baud_rate);
std::streamsize read_bin(std::ifstream& file, char* buffer, std::streamsize chunkSize);
bool send_bin(const OTA_Data& data); // 改为常量引用传递

