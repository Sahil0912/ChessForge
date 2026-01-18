#include "Board.hpp"

void Board::Initialize(){
    turn = Colors::White;
    for (int i = 16; i < 48; i++)
    {
        squares[i].type = Type::Empty;
        squares[i].color = Colors::None;
    }
    for (int i = 0; i < 8; i++)
    {
        squares[i].color = Colors::Black;
    }
    for (int i = 56; i < 64; i++)
    {
        squares[i].color = Colors::White;
    }
    for (int i = 8; i < 16; i++)
    {
        squares[i].type = Type::Pawn;
        squares[i].color = Colors::Black;
    }
    for (int i = 48; i < 56; i++)
    {
        squares[i].type = Type::Pawn;
        squares[i].color = Colors::White;
    }
    squares[0].type = squares[7].type = Type::Rook; 
    squares[1].type = squares[6].type = Type::Knight;
    squares[2].type = squares[5].type = Type::Bishop;
    squares[3].type = Type::Queen;
    squares[4].type = Type::King;

    squares[56].type = squares[63].type = Type::Rook; 
    squares[57].type = squares[62].type = Type::Knight;
    squares[58].type = squares[61].type = Type::Bishop;
    squares[59].type = Type::Queen;
    squares[60].type = Type::King;
    
}

Piece Board::GetPiece(int index) const {
    return squares[index];
}
