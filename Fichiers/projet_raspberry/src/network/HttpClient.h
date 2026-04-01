#pragma once
#include "data/Mesure.h"
#include <string>

class HttpClient {
public:
    static bool sendMesure(const Mesure& m);
    static std::string getLastError();

private:
    static std::string lastError;
};

