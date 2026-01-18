#pragma once
#include <raylib.h>
#include "../game/Board.hpp"
#include "../game/Piece.hpp"

class Renderer{
    private : 
        const int _Tilesize = 125;
        Texture2D _Pieces[2][7]; //color and which piece (0 is white and 1 is black)
        // mapping same as Type and Color
    public :
        Renderer() = default;
        ~Renderer() = default;
        void LoadAssets();
        void Draw(const Board& _Board);
        void UnloadAssets();
};