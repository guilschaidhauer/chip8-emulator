#include "GraphicsHandler/GraphicsHandler.h"
#include "Chip8/Chip8.h"
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_rom>" << std::endl;
        return 1; 
    }

    const char* romPath = argv[1];

    Chip8 chip8;
    if (!chip8.loadRom(romPath)) 
    {
        std::cerr << "Failed to load ROM: " << romPath << std::endl;
        return 1; 
    }

    GraphicsHandler graphicsHandler;
    graphicsHandler.init();

    const int INSTRUCTIONS_PER_SECOND = 500;
    const int TIMER_FREQUENCY = 60;

    std::chrono::milliseconds cycle_delay(1000 / INSTRUCTIONS_PER_SECOND);
    std::chrono::milliseconds timer_delay(1000 / TIMER_FREQUENCY);

    auto last_timer_update = std::chrono::high_resolution_clock::now();

    while (!WindowShouldClose()) 
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        chip8.cycle();

        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_timer_update) >= timer_delay) 
        {
            chip8.updateTimers();
            last_timer_update = now;
        }

        graphicsHandler.drawScreen(chip8.getScreenMatrix());

        auto end_time = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(cycle_delay - std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time));
    }

    graphicsHandler.close();

    return 0;
}
