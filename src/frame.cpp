#include "frame.h"

#include <vector>
#include <memory>
#include <system_error>
#include <bits/fs_fwd.h>
//
// Created by yunxiao on 3/13/25.
//

// 使用 unique_ptr 管理全局对象，避免静态初始化顺序问题
std::unique_ptr<MsgManager> frame_uart;
uint8_t frame_buffer[128];

bool frame_init(const char* device, speed_t baud_rate) {
    try {
        // 双重检查锁定（线程安全初始化）
        if (!frame_uart) {
            frame_uart = std::make_unique<MsgManager>(device, baud_rate);
        }
        return frame_uart->initialize();
    } catch (const std::exception& e) {
        std::cerr << "UART Init Error: " << e.what() << std::endl;
        return false;
    }
}

void SOC_Order_cmd(Order_Data *data, uint8_t hex_code, uint16_t crc,
                        uint8_t type, uint16_t freq,
                        uint8_t WSB_R, uint8_t WSB_G, uint8_t WSB_B, uint8_t Brightness,
                        uint32_t status_id) {
    data->frame_id = Frame_ID;
    data->version = Frame_VERSION;
    data->type = Type_Order;

    data->hex_code = hex_code;
    data->crc16 = crc;

    switch (hex_code) {
        case 0x31: break;
        case 0x32: break;
        case 0x33: break;
        case 0x34: break;
        default:break;
    }
    data->data_length = Frame_length + data->cmd_size;
}


bool data_pack(Order_Data *data) {
    // 参数校验
    if (!data ||!data->cmd_data) {
        return false;
    }

    // 头帧长度固定为10字节 + 数据区长度
    const size_t header_size = 10;
    const size_t total_size = header_size + data->cmd_size;
    
    // 写入帧头
    frame_buffer[0] = 0xEE;
    frame_buffer[1] = 0xEF;

    // 写入元数据
    frame_buffer[2] = data->frame_id;
    frame_buffer[3] = data->version;
    frame_buffer[4] = data->type;
    frame_buffer[5] = data->hex_code;

    // 处理cmd_size的字节序（假设协议要求大端序）
    frame_buffer[6] = static_cast<uint8_t>(data->cmd_size >> 8);
    frame_buffer[7] = static_cast<uint8_t>(data->cmd_size & 0xFF);

    // // 计算CRC（需实现crc16函数）
    // const uint16_t crc = crc16(buffer, total_size - 2); // 通常CRC不包含自身字段
    
    // // 写入CRC（大端序）
    // buffer[8] = static_cast<uint8_t>(crc >> 8);
    // buffer[9] = static_cast<uint8_t>(crc & 0xFF);

    frame_buffer[8] = data->crc16 >> 8;
    frame_buffer[9] = data->crc16 & 0xFF;

    // 拷贝命令数据
    memcpy(frame_buffer + header_size, data->cmd_data, data->cmd_size);

    return true;
}


bool send_frame( size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = write(frame_uart->serial_port, frame_buffer + total_sent, length - total_sent);
        if (sent == -1) {
            if (errno == EINTR) { // 被信号中断，重试
                continue;
            }
            std::cerr << "Write error: " << strerror(errno) << std::endl;
            return false;
        }
        total_sent += sent;
    }
    return true;
}



