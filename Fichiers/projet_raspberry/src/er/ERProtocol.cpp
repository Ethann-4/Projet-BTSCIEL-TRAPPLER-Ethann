#include "ERProtocol.h"

ERProtocol::ERProtocol(SerialPort& serialPort) : serial(serialPort) {
}

bool ERProtocol::sendTime(const std::string& date, const std::string& time) {
    return serial.writeLine("TIME:" + date + " " + time);
}

bool ERProtocol::sendRequest() {
    return serial.writeLine("REQUEST");
}

bool ERProtocol::sendOK() {
    return serial.writeLine("OK");
}

std::string ERProtocol::readFrame() {
    return serial.readLine();
}
