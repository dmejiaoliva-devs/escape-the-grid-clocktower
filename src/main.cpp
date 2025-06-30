#include "Game.h"

int main() {
    Game game;
    game.Initialize();
    
    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }
    
    CloseWindow();
    return 0;
}