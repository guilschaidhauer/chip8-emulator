#include "raylib.h"

int main() {
    // Initialize the window with width, height, and title
    InitWindow(800, 600, "Raylib Example");

    // Set the target FPS (frames per second)
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update the game logic here (e.g., handle input, update objects)

        // Start drawing
        BeginDrawing();
        
        ClearBackground(RAYWHITE);  // Clear the screen with a white color

        // Draw something on the screen
        DrawText("Hello, Raylib!", 190, 200, 20, RED);

        // End drawing
        EndDrawing();
    }

    // Deinitialize resources before exiting
    CloseWindow();  // Close window and OpenGL context

    return 0;
}
