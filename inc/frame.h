//
// Created by yunxiao on 3/12/25.
//
#include <msg_manager.h>

#include <cstdint>
#include <termios.h>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include <cstddef>

#ifndef FRAME_H
#define FRAME_H

#define Frame_length         10

#define Test_ID              0x25
#define Test_version         0x30
#define Test_type            0x35
#define Test_hex             0x40
#define Test_size            0x45
#define Test_crc             0x50

#define Frame_ID             0x01
#define Frame_VERSION        0x01

#define Type_req            0x11
#define Type_cmd            0x12
#define Type_Order          0x15

#define Hex_MCU             0x30
#define Hex_Light           0x31
#define Hex_Duoji           0x32

typedef struct {
    uint8_t header[2];
    uint8_t frame_id;//数据帧id
    uint8_t version;//版本
    uint8_t type;//数据帧类型
    uint8_t hex_code;//数据帧数据码
    uint16_t cmd_size = 0;//数据帧 自定义数据长度
    uint16_t crc16;//校验码

    uint8_t cmd_data[128];
    uint8_t data_length;
}Order_Data;


bool frame_init(const char* device, speed_t baud_rate);
bool data_pack(Order_Data *data, uint8_t* buffer);
bool send_frame( size_t length);




#endif //FRAME_H
