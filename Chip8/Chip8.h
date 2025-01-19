#include <cstdint>
#include <string>
#include <array>

class Chip8 
{
public:
    Chip8(); 
    ~Chip8();

    const std::array<bool, 64 * 32>& getScreenMatrix() const;

private:
    uint8_t V[16]; 
    uint16_t I, pc;
    uint8_t sp;

    uint16_t stack[16];

    uint8_t delay_timer, sound_timer;

    uint8_t memory[4096];

    std::array<bool, 64 * 32> screenMatrix {}; 

    int keypad[16];
};