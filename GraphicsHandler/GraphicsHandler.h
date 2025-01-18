#include "raylib.h"

class GraphicsHandler
{
public:
    GraphicsHandler();
    ~GraphicsHandler();

    void init();
    void close();
    void drawScreen(bool *screenMatrix);
     
private:
    const int screenWidth = 64;
    const int screenHeight = 32;
    const int screenScale = 16;
    const Color pixelColor = WHITE;

    void drawRectangleWithScale(int x, int y);
};