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

        // Print the address and the byte being loaded
        // printf("Loaded byte 0x%02X at address 0x%04X\n", (uint8_t) c, i);
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
    case 0x0:
    {
        switch (opcode)
        {
        case 0x00E0:
            screenMatrix.fill(false);
            pc += 2;
            break;
        
        case 0x00EE:
            sp--;
            pc = stack[sp];
            pc += 2;
            break;
        default:
            break;
        }
        break;
    }
    // 1 - 1NNN - Jumps to address NNN.
    case 0x1:
    {
        int address = opcode & 0x0FFF;        // Address (BCD)
        pc = address;
        break;
    }
    // 2 - 2NNN - Calls subroutine at NNN.
    case 0x2:
    {
        stack[sp] = pc;
        sp++;

        int address = opcode & 0x0FFF;        // Address (BCD)
        pc = address;
        break;
    }
    // 3 - 3XNN - Skips the next instruction if VX equals NN.
    case 0x3:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int lowestByte = opcode & 0x00FF;     // CD part
        
        pc += 2;
        
        if (V[nibbleB] == lowestByte)
        {
            pc += 2;
        }
        
        break;
    }
    // 4 - 4XNN - Skips the next instruction if VX equals NN.
    case 0x4:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int lowestByte = opcode & 0x00FF;     // CD part
        
        pc += 2;
        
        if (V[nibbleB] != lowestByte)
        {
            pc += 2;
        }
        
        break;
    }
    // 5 - 5XNN - Skips the next instruction if VX equals VY.
    case 0x5:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int nibbleC = (opcode & 0x00F0) >> 4; // Third nibble (C) 
        
        pc += 2;
        
        if (V[nibbleB] == V[nibbleC])
        {
            pc += 2;
        }
        
        break;
    }
    // 6 - 6XNN - Sets VX to NN.
    case 0x6:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int lowestByte = opcode & 0x00FF;     // CD part
        V[nibbleB] = lowestByte;
        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
        pc += 2;
        break;
    }
    // 7 - 7XNN - Adds NN to VX (carry flag is not changed).
    case 0x7:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int lowestByte = opcode & 0x00FF;     // CD part
        V[nibbleB] += lowestByte;
        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
        pc += 2;
        break;
    }
   // 9 - 9XY0 - Skips the next instruction if VX does not equal VY.
    case 0x9:
    {
        int nibbleB = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int nibbleC = (opcode & 0x00F0) >> 4; // Third nibble (C) 
        
        pc += 2;
        
        if (V[nibbleB] != V[nibbleC])
        {
            pc += 2;
        }
        
        break;
    }
    // 10 - ANNN - Sets I to the address NNN.
    case 0xA:
    {
        int address = opcode & 0x0FFF;        // Address (BCD)
        I = address;
        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
        pc += 2;
        break;
    }
    // 13 - DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
    case 0xD:
    {
        int x = (opcode & 0x0F00) >> 8;      // Second nibble (B)
        int y = (opcode & 0x00F0) >> 4;      // Third nibble (C)
        int height = opcode & 0x000F;        // Fourth nibble (D)

        printf("Drawing sprite at (V[%d]=%d, V[%d]=%d), height=%d, I=0x%04X\n", x, V[x], y, V[y], height, I);

        for (int i = 0; i < height; i++) {
            int byte = memory[I + i];
            printf("Sprite data[%d]: 0x%02X\n", i, memory[I + i]);
            drawPixelByte(V[x], V[y] + i, byte);
        }

        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
        pc += 2;
        break;
    }     
    default:
        break;
    }

    //pc += 2;
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
