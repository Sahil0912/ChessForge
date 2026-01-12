#include <raylib.h>
#include "game/Board.h"
#include "game/Piece.h"
#include "ui/Renderer.h"


int main(){

    const int ScreenSize = 1000;
    InitWindow(ScreenSize, ScreenSize, "Chess");
    Board _Board;
    _Board.Initialize();
    Renderer _Renderer;
    _Renderer.LoadAssets();
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        _Renderer.Draw(_Board);
        EndDrawing();
    }
    
    _Renderer.UnloadAssets();
    CloseWindow();
    return 0;
}