#include <cstdint>
#include <string>
#include <array>
#include <fstream>
#include <iostream>

class Chip8 
{
public:
    Chip8(); 
    ~Chip8();

    const std::array<bool, 64 * 32>& getScreenMatrix() const;

    bool loadRom(std::string path);

    void cycle();
    void updateTimers();

private:
    uint8_t V[16]; 
    uint16_t I, pc;
    uint8_t sp;

    uint16_t stack[16];

    uint8_t delayTimer, soundTimer;

    uint8_t memory[4096];

    std::array<bool, 64 * 32> screenMatrix {}; 

    int keypad[16];

    void drawPixelByte(int x, int y, int byte);

    int getNibble(int opcode, int bits, int toBinaryAnd);
};