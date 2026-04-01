#include <iostream>
#include <unistd.h>

#include "display/OLEDI2C.h"
#include "serial/SerialPort.h"
#include "er/ERProtocol.h"
#include "er/ERFrame.h"
#include "data/Mesure.h"
#include "utils/TimeUtils.h"
#include "network/HttpClient.h"
#include "utils/logger.h"

// Vérification simple
bool checkMesure(const Mesure& m) {
    return m.water >= 0 && m.energy >= 0;
}

// Affiche dans le terminal + écrit dans le log système
void infoStep(const std::string& msg) {
    std::cout << "[INFO] " << msg << std::endl;
    logSystem(msg);
}

// Affiche erreur dans le terminal + écrit dans error.log
void errorStep(const std::string& msg) {
    std::cerr << "[ERREUR] " << msg << std::endl;
    logError(msg);
}

int main() {

    infoStep("=== DEMARRAGE SYSTEME ===");

    // 🔷 INIT
    infoStep("Initialisation des objets...");
    OLEDI2C oled;
    SerialPort serial;

    infoStep("Initialisation OLED...");
    if (!oled.init()) {
        errorStep("Erreur init OLED");
        return 1;
    }

    infoStep("OLED initialise");

    infoStep("Ouverture du port serie /dev/serial0 ...");
    if (!serial.openPort("/dev/serial0", 9600)) {
        errorStep("Erreur ouverture port serie");
        return 1;
    }

    infoStep("Port serie ouvert");

    ERProtocol proto(serial);

    // 🔷 TIME
    infoStep("Recuperation date/heure systeme...");
    std::string date = TimeUtils::nowDate();
    std::string time = TimeUtils::nowTime();
    std::string fullTime = date + " " + time;

    infoStep("Envoi TIME...");
    if (!proto.sendTime(date, time)) {
        errorStep("Echec envoi TIME");
        return 1;
    }

    infoStep("TIME envoye : " + fullTime);

    usleep(200000); // 200 ms

    // 🔷 REQUEST
    infoStep("Envoi REQUEST...");
    if (!proto.sendRequest()) {
        errorStep("Echec envoi REQUEST");
        return 1;
    }

    infoStep("REQUEST envoye");

    // 🔷 RECEPTION DONNEES
    infoStep("Attente de la trame ER via serie...");
    std::string frame = proto.readFrame();

    infoStep("Trame brute recue : " + frame);

    infoStep("Parsing de la trame...");
    ERData er = ERFrame::parse(frame);

    if (!er.valid) {
        errorStep("Trame ER invalide");
        return 1;
    }

    infoStep("Trame ER valide");

    // 🔷 CREATION MESURE
    infoStep("Creation de la mesure...");
    Mesure m;
    m.water = er.water;
    m.energy = er.energy;
    m.date = date;
    m.time = time;
    m.valid = checkMesure(m);

    if (!m.valid) {
        errorStep("Mesure invalide");
        return 1;
    }

    infoStep("Mesure valide : eau=" + std::to_string(m.water) +
             " energie=" + std::to_string(m.energy));

    // 🔷 ENVOI HTTP
    infoStep("Debut envoi HTTP vers le serveur...");
    if (!HttpClient::sendMesure(m)) {
        errorStep("Echec envoi HTTP : " + HttpClient::getLastError());
    } else {
        infoStep("Donnees envoyees serveur");
    }

    // 🔷 AFFICHAGE OLED
    infoStep("Mise a jour affichage OLED...");
    oled.clear();

    oled.drawString(0, 0,  "Eau:");
    oled.drawString(50, 0, std::to_string(m.water));

    oled.drawString(0, 16, "Energie:");
    oled.drawString(70, 16, std::to_string((int)m.energy));

    oled.drawString(0, 32, "Heure:");
    oled.drawString(50, 32, time.substr(0, 5));

    oled.display();

    infoStep("Affichage OLED OK");

    // 🔷 OK
    infoStep("Envoi OK...");
    if (!proto.sendOK()) {
        errorStep("Echec envoi OK");
        return 1;
    }

    infoStep("OK envoye a la carte ER");

    // 🔷 FIN
    infoStep("FIN DU CYCLE - attente coupure");

    sleep(5);

    infoStep("Fin du programme");
    return 0;
}
