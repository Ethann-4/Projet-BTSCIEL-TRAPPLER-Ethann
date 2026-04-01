#include "OLEDI2C.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>

#define OLED_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 64

static unsigned char buffer[WIDTH * HEIGHT / 8];

// ------------------ POLICE 5x7 COMPATIBLE ------------------
// On déclare un tableau simple. Les indices non remplis seront à 0.
static unsigned char font5x7[128][5];

void fillFont() {
    // On vide le tableau au cas où
    memset(font5x7, 0, sizeof(font5x7));

    // On remplit manuellement les caractères nécessaires
    // Chiffres
    unsigned char n0[] = {0x3E,0x51,0x49,0x45,0x3E}; memcpy(font5x7['0'], n0, 5);
    unsigned char n1[] = {0x00,0x42,0x7F,0x40,0x00}; memcpy(font5x7['1'], n1, 5);
    unsigned char n2[] = {0x42,0x61,0x51,0x49,0x46}; memcpy(font5x7['2'], n2, 5);
    unsigned char n3[] = {0x21,0x41,0x45,0x4B,0x31}; memcpy(font5x7['3'], n3, 5);
    unsigned char n4[] = {0x18,0x14,0x12,0x7F,0x10}; memcpy(font5x7['4'], n4, 5);
    unsigned char n5[] = {0x27,0x45,0x45,0x45,0x39}; memcpy(font5x7['5'], n5, 5);
    unsigned char n6[] = {0x3C,0x4A,0x49,0x49,0x30}; memcpy(font5x7['6'], n6, 5);
    unsigned char n7[] = {0x01,0x71,0x09,0x05,0x03}; memcpy(font5x7['7'], n7, 5);
    unsigned char n8[] = {0x36,0x49,0x49,0x49,0x36}; memcpy(font5x7['8'], n8, 5);
    unsigned char n9[] = {0x06,0x49,0x49,0x29,0x1E}; memcpy(font5x7['9'], n9, 5);

    // Majuscules
    unsigned char A[] = {0x7E,0x11,0x11,0x11,0x7E}; memcpy(font5x7['A'], A, 5);
    unsigned char B[] = {0x7F,0x49,0x49,0x49,0x36}; memcpy(font5x7['B'], B, 5);
    unsigned char C[] = {0x3E,0x41,0x41,0x41,0x22}; memcpy(font5x7['C'], C, 5);
    unsigned char E[] = {0x7F,0x49,0x49,0x49,0x41}; memcpy(font5x7['E'], E, 5);
    unsigned char G[] = {0x3E,0x41,0x49,0x49,0x7A}; memcpy(font5x7['G'], G, 5);
    unsigned char I[] = {0x00,0x41,0x7F,0x41,0x00}; memcpy(font5x7['I'], I, 5);
    unsigned char L[] = {0x7F,0x40,0x40,0x40,0x40}; memcpy(font5x7['L'], L, 5);
    unsigned char N[] = {0x7F,0x02,0x04,0x08,0x7F}; memcpy(font5x7['N'], N, 5);
    unsigned char O[] = {0x3E,0x41,0x41,0x41,0x3E}; memcpy(font5x7['O'], O, 5);
    unsigned char R[] = {0x7F,0x09,0x19,0x29,0x46}; memcpy(font5x7['R'], R, 5);
    unsigned char T[] = {0x01,0x01,0x7F,0x01,0x01}; memcpy(font5x7['T'], T, 5);
    unsigned char U[] = {0x3F,0x40,0x40,0x40,0x3F}; memcpy(font5x7['U'], U, 5);
    unsigned char K[] = {0x7F, 0x08, 0x14, 0x22, 0x41}; memcpy(font5x7['K'], K, 5);
    unsigned char H[] = {0x7F, 0x08, 0x08, 0x08, 0x7F}; memcpy(font5x7['H'], H, 5);


    // Minuscules
    unsigned char a[] = {0x20,0x54,0x54,0x54,0x78}; memcpy(font5x7['a'], a, 5);
    unsigned char e[] = {0x38,0x54,0x54,0x54,0x18}; memcpy(font5x7['e'], e, 5);
    unsigned char g[] = {0x0C,0x52,0x52,0x52,0x3E}; memcpy(font5x7['g'], g, 5);
    unsigned char i[] = {0x00,0x44,0x7D,0x40,0x00}; memcpy(font5x7['i'], i, 5);
    unsigned char n[] = {0x7C,0x04,0x04,0x04,0x78}; memcpy(font5x7['n'], n, 5);
    unsigned char r[] = {0x7C,0x08,0x04,0x04,0x08}; memcpy(font5x7['r'], r, 5);
    unsigned char t[] = {0x04,0x3F,0x44,0x40,0x20}; memcpy(font5x7['t'], t, 5);
    unsigned char u[] = {0x3C,0x40,0x40,0x20,0x7C}; memcpy(font5x7['u'], u, 5);
    unsigned char h_min[] = {0x7F, 0x08, 0x04, 0x04, 0x78}; memcpy(font5x7['h'], h_min, 5);




    // Symboles
    unsigned char sep[] = {0x00,0x36,0x36,0x00,0x00}; memcpy(font5x7[':'], sep, 5);
    unsigned char dot[] = {0x00,0x40,0x60,0x00,0x00}; memcpy(font5x7['.'], dot, 5);
}

// ------------------ I2C ------------------

OLEDI2C::OLEDI2C() : fd(-1) {}
OLEDI2C::~OLEDI2C() { if (fd >= 0) close(fd); }

void sendCommand(int fd, unsigned char cmd) {
    unsigned char buf[2] = {0x00, cmd};
    write(fd, buf, 2);
}

void sendData(int fd, unsigned char* data, int size) {
    unsigned char buf[size + 1];
    buf[0] = 0x40;
    memcpy(&buf[1], data, size);
    write(fd, buf, size + 1);
}

// ------------------ INIT ------------------
bool OLEDI2C::init() {
    fillFont(); // On initialise la police
    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) return false;
    if (ioctl(fd, I2C_SLAVE, OLED_ADDR) < 0) return false;

    unsigned char initCmds[] = {
        0xAE, 0x20, 0x00, 0xB0, 0xC8, 0x00, 0x10, 0x40,
        0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F, 0xA4, 0xD3,
        0x00, 0xD5, 0xF0, 0xD9, 0x22, 0xDA, 0x12, 0xDB,
        0x20, 0x8D, 0x14, 0xAF
    };
    for(size_t i = 0; i < sizeof(initCmds); i++) {
        sendCommand(fd, initCmds[i]);
    }

    clear();
    display();
    return true;
}

void OLEDI2C::clear() {
    memset(buffer, 0x00, sizeof(buffer));
}

// ------------------ TEXTE ------------------

void OLEDI2C::drawString(int x, int y, const std::string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        unsigned char c = (unsigned char)str[i];
        int px = x + (i * 6);
        if (px + 5 >= WIDTH) break;

        for (int col = 0; col < 5; col++) {
            unsigned char line = font5x7[c][col];
            for (int row = 0; row < 8; row++) {
                if (line & (1 << row)) {
                    int py = y + row;
                    if (py < HEIGHT) {
                        int index = (px + col) + (py / 8) * WIDTH;
                        buffer[index] |= (1 << (py % 8));
                    }
                }
            }
        }
    }
}

void OLEDI2C::display() {
    for (int page = 0; page < 8; page++) {
        sendCommand(fd, 0xB0 + page);
        sendCommand(fd, 0x00);
        sendCommand(fd, 0x10);
        sendData(fd, &buffer[WIDTH * page], WIDTH);
    }
}

