#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
}

GraphicsHandler::~GraphicsHandler()
{
}

void GraphicsHandler::init()
{
    InitWindow(screenWidth * screenScale, screenHeight  * screenScale, "RAY8 - CHIP8 Emulator");
}

void GraphicsHandler::close()
{
    CloseWindow();  
}

void GraphicsHandler::drawRectangleWithScale(int x, int y)
{
    DrawRectangle(x * screenScale, y * screenScale, screenScale, screenScale, pixelColor);
}

void GraphicsHandler::drawScreen(std::array<bool, 64 * 32> screenMatrix)
{
    BeginDrawing();
    
    ClearBackground(Color{130, 5, 50, 255}); 

    int y = 0;

    for (int i=0; i<2048; i++)
    {
        if (screenMatrix[i]) 
        {
            y = i / screenWidth;
            int x = i - y * screenWidth; 
            drawRectangleWithScale(x, y);
        }
    }

    EndDrawing();
}