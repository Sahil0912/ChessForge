#include "Renderer.hpp"

void Renderer::LoadAssets(){
    _Pieces[(int)Colors::White][(int)Type::Pawn] = LoadTexture("assets/white-pawn.png");
    _Pieces[(int)Colors::White][(int)Type::Bishop] = LoadTexture("assets/white-bishop.png");
    _Pieces[(int)Colors::White][(int)Type::Knight] = LoadTexture("assets/white-knight.png");
    _Pieces[(int)Colors::White][(int)Type::Rook] = LoadTexture("assets/white-rook.png");
    _Pieces[(int)Colors::White][(int)Type::Queen] = LoadTexture("assets/white-queen.png");
    _Pieces[(int)Colors::White][(int)Type::King] = LoadTexture("assets/white-king.png");

    _Pieces[(int)Colors::Black][(int)Type::Pawn] = LoadTexture("assets/black-pawn.png");
    _Pieces[(int)Colors::Black][(int)Type::Bishop] = LoadTexture("assets/black-bishop.png");
    _Pieces[(int)Colors::Black][(int)Type::Knight] = LoadTexture("assets/black-knight.png");
    _Pieces[(int)Colors::Black][(int)Type::Rook] = LoadTexture("assets/black-rook.png");
    _Pieces[(int)Colors::Black][(int)Type::Queen] = LoadTexture("assets/black-queen.png");
    _Pieces[(int)Colors::Black][(int)Type::King] = LoadTexture("assets/black-king.png");

}
void Renderer::UnloadAssets(){
    for(int c = 0; c < 2; c++){
        for(int t = 1; t < 7; t++){
            UnloadTexture(_Pieces[c][t]);
        }
    }
    
}
void Renderer::Draw(const Board& _Board){
    for(int file = 0; file < 8; file++){
        for(int row = 0; row < 8; row++){
            DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, (file + row) % 2 ? BLUE : WHITE);
            Piece _piece = _Board.GetPiece(row * 8 + file);
            if(_piece.type != Type::Empty)
                DrawTexture(_Pieces[(int)_piece.color][(int)_piece.type], file * _Tilesize, row * _Tilesize, WHITE);
        }
    }
}