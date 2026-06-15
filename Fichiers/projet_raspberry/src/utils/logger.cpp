#include "logger.h"
#include <fstream>
#include <ctime>

std::string getTime() {
    time_t now = time(nullptr); // Récupère le temps actuel brut du Raspberry Pi.
    char buf[64];               // Prépare un tableau de caractères pour stocker le texte final.
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now)); // Traduit le temps brut en heure locale et le formate proprement
    return std::string(buf);    // Transforme le tableau de texte en std::string et le renvoie.
}

void logSystem(const std::string& message) {
    std::ofstream log("logs/system.log", std::ios::app);
    log << getTime() << " | " << message << std::endl;
}

void logError(const std::string& message) {
    std::ofstream log("logs/error.log", std::ios::app);
    log << getTime() << " | " << message << std::endl;
}
