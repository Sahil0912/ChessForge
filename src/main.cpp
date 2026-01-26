#include <raylib.h>
#include "game/Board.hpp"
#include "game/Piece.hpp"
#include "ui/Renderer.hpp"


int main(){

    const int ScreenSize = 1000;
    InitWindow(ScreenSize, ScreenSize, "Chess");
    Board _Board;
    _Board.Initialize();
    Renderer _Renderer;
    _Renderer.LoadAssets();
    SetTargetFPS(60);
    int stateOfApp = 0; // 0 - Menu, 1 - pVp, 2 - pVstockfish 
    while (!WindowShouldClose())
    {
        _Renderer.HandleInput(_Board, stateOfApp);

        BeginDrawing();
        _Renderer.Draw(_Board, stateOfApp);
        EndDrawing();
    }
    
    _Renderer.UnloadAssets();
    CloseWindow();
    return 0;
}