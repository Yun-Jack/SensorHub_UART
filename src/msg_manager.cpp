//
// Created by yunxiao on 3/10/25.
//
#include "msg_manager.h"


using namespace std;

// uint8_t temp_buffer[256];//temp data
char temp_buffer[256];//temp data

MsgManager::MsgManager(const char* device, speed_t baud_rate)
    : serial_port(-1), device(device), baud_rate(baud_rate) {
    // std::cout << "tty open "<< device <<std::endl;
}

MsgManager::~MsgManager(void) {
    if (serial_port >= 0) {
        close(serial_port);
        std::cout << "tty close " << device <<std::endl;
    }
}


bool MsgManager::initialize() {
    serial_port = open(device, O_RDWR);
    if (serial_port < 0) {
        std::cerr << "Error " << errno << " opening " << device << ": " << strerror(errno) << std::endl;
        return false;
    }
    configure_serial_port();
    return serial_port;
}

void MsgManager::run() {
    read_serial_data();
}

bool MsgManager::send_message(uint8_t* data, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = write(serial_port, data + total_sent, length - total_sent);
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




void MsgManager::read_serial_data() {

    int length_02 = read(serial_port, &temp_buffer, sizeof(temp_buffer));

    uint8_t print_buffer[length_02];
    for (int j = 0; j < length_02; j++) {
        print_buffer[j] = temp_buffer[j];
        if (print_buffer[j] == '\0') {
            j = length_02;
        };
    }
    std::cout<<print_buffer<<std::endl;

}

bool MsgManager::configure_serial_port() {
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
        return false;
    }

    cfsetispeed(&tty, baud_rate);
    cfsetospeed(&tty, baud_rate);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICRNL | INLCR);
    tty.c_iflag &= ~(IGNCR);

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        std::cerr << "Error " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

