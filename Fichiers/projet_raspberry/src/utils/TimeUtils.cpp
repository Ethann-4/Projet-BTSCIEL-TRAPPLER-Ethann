#include "TimeUtils.h"
#include <ctime>
#include <iomanip>
#include <sstream>

std::string TimeUtils::nowDate() {
    std::time_t t = std::time(nullptr);
    std::tm *tm = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

std::string TimeUtils::nowTime() {
    std::time_t t = std::time(nullptr);
    std::tm *tm = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm, "%H:%M:%S");
    return oss.str();
}
