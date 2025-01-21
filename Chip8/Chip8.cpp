#include "Chip8.h"

Chip8::Chip8()
{
}

Chip8::~Chip8()
{
}

const std::array<bool, 64 * 32> &Chip8::getScreenMatrix() const
{
    return screenMatrix;
}

bool Chip8::loadRom(std::string path)
{
    std::ifstream file(path, std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        return false;
    }

    char c;
    int memoryLimit = 0xFFF; // 4095

    for (int i=0x200; file.get(c); i++) {
        memory[i] = (uint8_t) c;
    }

    return true;
}
