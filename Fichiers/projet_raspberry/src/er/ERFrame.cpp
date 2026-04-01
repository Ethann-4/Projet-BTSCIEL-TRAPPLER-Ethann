#include "ERFrame.h"
#include <sstream>

ERData ERFrame::parse(const std::string& frame) {
    ERData data{0, 0.0f, false};

    if (frame.find("WATER=") == std::string::npos) return data;

    sscanf(frame.c_str(), "WATER=%d;ENERGY=%f", &data.water, &data.energy);
    data.valid = true;
    return data;
}


