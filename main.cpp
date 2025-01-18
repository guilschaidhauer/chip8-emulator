#include "GraphicsHandler.h"

int main() 
{
    bool screenMatrix[2048] = { 0 };

    GraphicsHandler graphicsHandler;
    graphicsHandler.init();

    // Main game loop
    while (!WindowShouldClose()) 
    {
        graphicsHandler.drawScreen(screenMatrix); 
    }

    graphicsHandler.close();

    return 0;
}
