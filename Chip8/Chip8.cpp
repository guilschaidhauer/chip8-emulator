#include "Chip8.h"

Chip8::Chip8()
{
    pc = 0x200;
    I = 0;
    sp = 0;

    delayTimer = 0;
    soundTimer = 0;

    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(memory, 0, sizeof(memory));
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

void Chip8::cycle()
{
    if (pc > 0xFFF) {
        return;
    }

    int opcode = (memory[pc] << 8) | (memory[pc + 1]);;

    int opcodeMsbNibble = getNibble(opcode, 12, 0xF000);

    printf("%d\n", opcode);
    printf("%d\n", opcodeMsbNibble);

    switch (opcodeMsbNibble)
    {
    // Sets VX to NN.
    case 6:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int lowestByte = opcode & 0x00FF;     // CD part
        V[nibbleB] = lowestByte;
        printf("%d\n", nibbleB);
        printf("%d\n", lowestByte);
        break;
    }   
    default:
        break;
    }

    pc += 2;
    if (pc > 0xFFF) {
        pc = 0x200;
    }
}

int Chip8::getNibble(int opcode, int bits, int toBinaryAnd)
{
    return ((opcode & toBinaryAnd) >> bits);
}
