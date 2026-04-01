#pragma once
#include <string>
#include "serial/SerialPort.h"

class ERProtocol {
public:
    explicit ERProtocol(SerialPort& serialPort);

    bool sendTime(const std::string& date, const std::string& time);
    bool sendRequest();
    bool sendOK();
    std::string readFrame();

private:
    SerialPort& serial;
};





