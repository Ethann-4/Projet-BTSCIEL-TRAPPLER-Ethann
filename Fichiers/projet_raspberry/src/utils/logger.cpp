#include "logger.h"
#include <fstream>
#include <ctime>

std::string getTime() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

void logSystem(const std::string& message) {
    std::ofstream log("logs/system.log", std::ios::app);
    log << getTime() << " | " << message << std::endl;
}

void logError(const std::string& message) {
    std::ofstream log("logs/error.log", std::ios::app);
    log << getTime() << " | " << message << std::endl;
}
