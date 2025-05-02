//
// Created by yunxiao on 3/10/25.
//

#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

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




class MsgManager {
public:
    MsgManager(const char* device, speed_t baud_rate);
    ~MsgManager();

    bool initialize();
    void run();
    bool send_message(uint8_t* data, size_t length);


    int serial_port;
    
private:
    const char* device;
    speed_t baud_rate;
    struct termios tty;

    void process_message(const uint8_t* message, uint8_t length);
    void read_serial_data();
    bool configure_serial_port();

    uint8_t cmd_buffer[128];

    static constexpr uint8_t FRAME_HEADER1 = 0xEE;
    static constexpr uint8_t FRAME_HEADER2 = 0xEF;
    static constexpr uint8_t FRAME_TAIL1 = 0xEF;
    static constexpr uint8_t FRAME_TAIL2 = 0xFF;
};


#endif //MSG_MANAGER_H
