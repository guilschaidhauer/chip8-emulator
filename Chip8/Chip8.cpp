#include "Chip8.h"

Chip8::Chip8()
{
    unsigned char chip8Fontset[80] =
    {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    pc = 0x200;
    I = 0;
    sp = 0;

    delayTimer = 0;
    soundTimer = 0;

    for (int i = 0; i < 80; i++)
    {
        memory[i] = chip8Fontset[i];
    }

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
    // 8 
    case 0x8:
    {
        int x = (opcode & 0x0F00) >> 8; // Second nibble (B)
        int y = (opcode & 0x00F0) >> 4; // Third nibble (C)
        int nibbleD = opcode & 0x000F;  // Fourth nibble (D)
        
        switch (nibbleD)
        {
        // 8XY0 - Sets VX to the value of VY.
        case 0:
            V[x] = V[y];
            pc += 2;
            break;
        // 8XY1 - Sets VX to VX or VY. (bitwise OR operation).
        case 1:
            V[x] |= V[y];
            V[0xF] = 0;
            pc += 2;
            break;
        // 8XY2 - Sets VX to VX and VY. (bitwise AND operation).
        case 2:
            V[x] &= V[y];
            V[0xF] = 0;
            pc += 2;
            break;
        // 8XY3 - Sets VX to VX xor VY.
        case 3:
            V[x] ^= V[y];
            V[0xF] = 0;
            pc += 2;
            break;
        // 8XY4 - Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
        case 4:
            V[x] += V[y]; 
            if (V[x] > 0xFF)
            {
                V[0xF] = 1;
            } 
            else 
            {
                V[0xF] = 0;
            }
            V[x] &= 0xFF; 
            pc += 2;
            break;
        // 8XY5 - VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
        case 5:
        {
            uint8_t vx = V[x]; 
            uint8_t vy = V[y]; 

            V[0xF] = (vx >= vy) ? 1 : 0;  

            V[x] = vx - vy;  

            pc += 2;
            break;
        }
        // 8XY6 - Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
        case 6: 
            V[0xf] = V[x] & 0x1;
            V[x] >>= 1;
            pc += 2;
            break;
        // 8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
        case 7:
            if (V[y] > V[x])
            {
                V[0xF] = 1;
            } 
            else 
            {
                V[0xF] = 0;
            }
            V[x] = V[y] - V[x];
            pc += 2;
            break;
        // 8XYE - Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
        case 0xE:
            V[0xf] = V[x] >> 7;
            V[x] <<= 1;
            pc += 2;
            break;
        default:
            break;
        }

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
    // 11 - BNNN - Jumps to the address NNN plus V0.
    case 0xB:
    {
        int address = opcode & 0x0FFF;        // Address (BCD)
        I = address + V[0];
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

        // printf("Drawing sprite at (V[%d]=%d, V[%d]=%d), height=%d, I=0x%04X\n", x, V[x], y, V[y], height, I);

        for (int i = 0; i < height; i++) {
            int byte = memory[I + i];
            // printf("Sprite data[%d]: 0x%02X\n", i, memory[I + i]);
            drawPixelByte(V[x], V[y] + i, byte);
        }

        //printf("%d\n", nibbleB);
        //printf("%d\n", lowestByte);
        pc += 2;
        break;
    }
    case 0xE:
    {
        int nibbleCD = opcode & 0x00FF;
        int x = (opcode & 0x0F00) >> 8;      // Second nibble (B)

        switch (nibbleCD)
        {
            // EX9E - Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed.
            case 0x9E:
            {
                pc += 2;
                if (keypad[V[x]] != 0)
                {
                    pc += 2;
                }
                break;
            }
            // EXA1
            case 0xA1:
            {
                pc += 2;
                if (keypad[V[x]] == 0)
                {
                    pc += 2;
                }
                break;
            }
        } 

        break;
    }
     // 15
    case 0xF:
    {
        int nibbleCD = opcode & 0x00FF;
        int x = (opcode & 0x0F00) >> 8;      // Second nibble (B)
        switch (nibbleCD)
        {
        // FX07 - Sets VX to the value of the delay timer.
        case 0x07:
        {
            V[x] = delayTimer;
            pc += 2;
            break;
        }
         // FX0A - A key press is awaited, and then stored in VX
        case 0x0A:
        {
            bool keyPressed = false;
            for (int i = 0; i < 16; i++)
            {
                if (keypad[i] != 0)
                {
                    keyPressed = true;
                    V[x] = (uint8_t) i;
                }
            }
            if (keyPressed)
            {
                pc += 2;
            }
            break;
        }
        // FX15 - Sets the delay timer to VX.
        case 0x15:
        {
            delayTimer = V[x];
            pc += 2;
            break;
        }
         // FX18 - Sets the sound timer to VX.
        case 0x18:
        {
            soundTimer = V[x];
            pc += 2;
            break;
        }
        // FX1E - Adds VX to I. VF is not affected.
        case 0x1E:
        {
            I += V[x];
            pc += 2;
            break;
        }
        // FX29 - Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font.
        case 0x29:
        {
            I = V[x] * 0x5;
            pc += 2;
            break;
        }
        // FX33 - Store BCD representation of VX at memory[I]
        case 0x33:
        {
            memory[I] = V[x] / 100;
            memory[I+1] = (V[x] / 10) % 10;
            memory[I+2] = V[x] % 10;
            pc += 2;
            break;
        }
        // FX55 - Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
        case 0x55:
        {
            for (int i=0; i<=x; i++) {
                memory[I+i] = V[i];
            }
            pc += 2;
            break;
        }
        // FX65 - Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
        case 0x65:
        {
            for (int i=0; i<=x; i++) {
                V[i] = memory[I+i];
            }
            pc += 2;
            break;
        }
        default:
            break;
        }
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

void Chip8::updateTimers()
{
    if (delayTimer > 0) 
        delayTimer--;

    if (soundTimer > 0) 
    {
        // Play sound here
        soundTimer--;
    }
}

void Chip8::setKeypadValue(int index, int val)
{
    keypad[index] = val;
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
