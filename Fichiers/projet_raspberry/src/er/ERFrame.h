
#pragma once
#include <string>

struct ERData {
    int water;
    float energy;
    bool valid;
};

class ERFrame {
public:
    static ERData parse(const std::string& frame);
};





