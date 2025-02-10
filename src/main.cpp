#include <raylib.h>
#include "game.hpp"

int main() {
    const Color darkGreen = {20, 160, 133, 255};
    
    constexpr int screenWidth = 750;
    constexpr int screenHeight = 700;
    
    
    InitWindow(screenWidth, screenHeight, "Space Invaders");
    SetTargetFPS(60);
    
    Game game;

    while (!WindowShouldClose()){
        game.HandleInput();
        game.Update();

        BeginDrawing();

        ClearBackground(darkGreen);
        game.Draw();

        EndDrawing();
    }
    
    CloseWindow();
}