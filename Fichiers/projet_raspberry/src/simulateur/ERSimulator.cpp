#include "ERSimulator.h"
#include <cstdlib>

std::string ERSimulator::generateFrame() {
    int water = rand() % 5000;
    float energy = (rand() % 1000) / 10.0;
    return "WATER=" + std::to_string(water) +
           ";ENERGY=" + std::to_string(energy) + "\n";
}
