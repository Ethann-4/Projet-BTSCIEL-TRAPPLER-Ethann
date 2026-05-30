#include "SerialPort.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

SerialPort::SerialPort() : fd(-1) {
}

SerialPort::~SerialPort() {
    closePort();
}

bool SerialPort::openPort(const std::string& device, int baudrate) {
    fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Erreur ouverture port serie");
        return false;
    }

    fcntl(fd, F_SETFL, 0);

    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        perror("Erreur tcgetattr");
        closePort();
        return false;
    }

    speed_t speed = B9600;
    switch (baudrate) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 115200: speed = B115200; break;
        default:
            std::cerr << "Baudrate non supporte: " << baudrate << std::endl;
            closePort();
            return false;
    }

    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);

    // 8N1
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Mode brut
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~OPOST;

    // Lecture non bloquante en attente courte
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 1; // 0.1 s

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("Erreur tcsetattr");
        closePort();
        return false;
    }

    tcflush(fd, TCIOFLUSH);

    return true;
}

void SerialPort::closePort() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

bool SerialPort::writeLine(const std::string& line) {
    if (fd < 0) {
        return false;
    }

    std::string msg = line + "\n";
    ssize_t totalWritten = 0;
    ssize_t toWrite = static_cast<ssize_t>(msg.size());

    while (totalWritten < toWrite) {
        ssize_t n = write(fd, msg.c_str() + totalWritten, toWrite - totalWritten);
        if (n < 0) {
            perror("Erreur ecriture serie");
            return false;
        }
        totalWritten += n;
    }

    tcdrain(fd);
    return true;
}

std::string SerialPort::readLine() {
    std::string line;

    if (fd < 0) {
        return line;
    }

    char c;
    while (true) {
        ssize_t n = read(fd, &c, 1);

        if (n > 0) {
            if (c == '\n') {
                break;
            }
            if (c != '\r') {
                line += c;
            }
        } else {
            usleep(10000); // 10 ms
        }
    }

    return line;
}

bool SerialPort::isOpen() const {
    return fd >= 0;
}
