#include <OTA.h>

#include <vector>
#include <memory>
#include <system_error>

// 使用 unique_ptr 管理全局对象，避免静态初始化顺序问题
std::unique_ptr<MsgManager> OTA_uart;

bool OTA_init(const char* device, speed_t baud_rate) {
    try {
        // 双重检查锁定（线程安全初始化）
        if (!OTA_uart) {
            OTA_uart = std::make_unique<MsgManager>(device, baud_rate);
        }
        return OTA_uart->initialize();
    } catch (const std::exception& e) {
        std::cerr << "UART Init Error: " << e.what() << std::endl;
        return false;
    }
}

std::streamsize read_bin(std::ifstream& file, char* buffer, std::streamsize chunkSize) {
    // 参数校验增强
    if (chunkSize < 0) throw std::invalid_argument("chunkSize cannot be negative");
    if (!buffer) throw std::invalid_argument("buffer is nullptr");
    if (!file) throw std::ios_base::failure("File stream is in bad state");

    file.read(buffer, chunkSize);
    const auto bytesRead = file.gcount();

    // 区分 EOF 和错误
    if (file.eof()) {
        std::cout << "Reached end of file" << std::endl;
    } else if (file.fail() && !file.eof()) {
        throw std::ios_base::failure("Partial read occurred");
    }

    return bytesRead;
}

bool send_bin(const OTA_Data& data) {
    // 参数校验
    if (data.bin_length == 0 || !data.bin_data) {
        throw std::invalid_argument("Invalid OTA_Data: empty payload");
    }

    // 零拷贝优化：直接使用原始数据指针
    const char* p = data.bin_data;
    size_t remaining = data.bin_length;
    ssize_t total_written = 0;

    while (remaining > 0) {
        const ssize_t written = write(OTA_uart->serial_port, p + total_written, remaining);
        if (written == -1) {
            if (errno == EINTR) continue;
            throw std::system_error(errno, std::system_category(), "Serial write failed");
        }
        total_written += written;
        remaining -= written;
    }

    // 性能统计（可选）
    std::cout << "Transferred " << total_written << " bytes ("
              << (total_written / 1024.0 / 1024.0) << " MB)" << std::endl;
    return true;
}

