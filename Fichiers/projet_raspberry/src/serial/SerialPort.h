#pragma once
#include <string>

class SerialPort {
public:
    SerialPort();
    ~SerialPort();

    bool openPort(const std::string& device, int baudrate);
    void closePort();

    bool writeLine(const std::string& line);
    std::string readLine();

    bool isOpen() const;

private:
    int fd;
};
