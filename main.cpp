#include "GraphicsHandler/GraphicsHandler.h"
#include "Chip8/Chip8.h"
#include <iostream>

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

    while (!WindowShouldClose()) 
    {
        chip8.cycle();
        graphicsHandler.drawScreen(chip8.getScreenMatrix());
    }

    graphicsHandler.close();

    return 0;
}
