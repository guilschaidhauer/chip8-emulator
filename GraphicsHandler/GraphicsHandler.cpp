#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
}

GraphicsHandler::~GraphicsHandler()
{
}

void GraphicsHandler::init()
{
    // Initialize the window with width, height, and title
    InitWindow(screenWidth * screenScale, screenHeight  * screenScale, "Raylib Example");
}

void GraphicsHandler::close()
{
    // Deinitialize resources before exiting
    CloseWindow();  // Close window and OpenGL context
}

void GraphicsHandler::drawRectangleWithScale(int x, int y)
{
    DrawRectangle(x * screenScale, y * screenScale, screenScale, screenScale, pixelColor);
}

void GraphicsHandler::drawScreen(std::array<bool, 64 * 32> screenMatrix)
{
    // Start drawing
    BeginDrawing();
    
    //ClearBackground(Color{56, 82, 110, 0}); 

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

    // End drawing
    EndDrawing();
}