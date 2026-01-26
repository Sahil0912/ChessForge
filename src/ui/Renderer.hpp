#pragma once
#include <raylib.h>
#include "../game/Board.hpp"
#include <algorithm>


class Renderer{
    private : 
        const int _Tilesize = 125;
        Texture2D _Pieces[2][7]; //color and which piece (0 is white and 1 is black)
        // mapping same as Type and Color
        int selectedSquare;
        bool isPromoting;
        Move pendingMove; //for promotion thing
    public :
        Renderer() : selectedSquare(-1), isPromoting(false) {}
        ~Renderer() = default;
        void LoadAssets();
        void Draw(Board& _Board, int stateOfApp = 1);
        void HandleInput(Board& _Board);
        void HandleInput(Board& _Board, int &stateOfApp);
        void UnloadAssets();
};