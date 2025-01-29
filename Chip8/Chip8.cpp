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

    //printf("%d\n", opcode);
    //printf("%d\n", opcodeMsbNibble);

    switch (opcodeMsbNibble)
    {
    // 6 - 6XNN - Sets VX to NN.
    case 0x6:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int lowestByte = opcode & 0x00FF;     // CD part
        V[nibbleB] = lowestByte;
        printf("%d\n", nibbleB);
        printf("%d\n", lowestByte);
        break;
    }
    // 10 - ANNN - Sets I to the address NNN.
    case 0xA:
    {
        int address = opcode & 0x0FFF;        // Address (BCD)
        I = address;
        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
        break;
    }
    // 13 - DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
    case 0xD:
    {
        int x = (opcode & 0x0F00) >> 8;      // Second nibble (B)
        int y = (opcode & 0x00F0) >> 4;      // Third nibble (C)
        int height = opcode & 0x000F;        // Fourth nibble (D)

        // Find a way to use the sprite data to draw it
        for (int i = 0; i < height; i++) {
            int byte = memory[I + i];
            drawPixelByte(V[x], V[y] + i, byte);
        }

        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
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

void Chip8::drawPixelByte(int x, int y, int byte)
{
    for (int bit = 7; bit >= 0; bit--)
    {
        bool isPixelOn = (byte & (1 << bit)) != 0;
        if (isPixelOn)
        {
            // The pixel is ON
            int pixelIndex = y * 64 + (x + 7 - bit);

            // XOR operator
            if (screenMatrix[pixelIndex]) {
                screenMatrix[pixelIndex] = false;  
            } else {
                screenMatrix[pixelIndex] = true; 
            }
        }
    }
}

int Chip8::getNibble(int opcode, int bits, int toBinaryAnd)
{
    return ((opcode & toBinaryAnd) >> bits);
}
