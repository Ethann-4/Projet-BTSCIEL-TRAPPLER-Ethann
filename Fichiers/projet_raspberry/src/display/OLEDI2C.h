#pragma once
#include <string>

class OLEDI2C {
public:
    OLEDI2C();
    ~OLEDI2C(); // Pour fermer le fichier I2C proprement
    bool init();
    void clear();
    void drawString(int x, int y, const std::string& str);
    void display();

private:
    int fd;
};



