#include "GraphicsHandler/GraphicsHandler.h"
#include "Chip8/Chip8.h"

int main() 
{
    Chip8 chip8;

    GraphicsHandler graphicsHandler;
    graphicsHandler.init();

    while (!WindowShouldClose()) 
    {
        graphicsHandler.drawScreen(chip8.getScreenMatrix()); 
    }

    graphicsHandler.close();

    return 0;
}
