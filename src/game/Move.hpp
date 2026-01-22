#pragma once

#include "Piece.hpp"

struct Move
{
    int startSquare;
    int endSquare;
    Type promotionPiece;
    bool operator==(const Move &move) const{
        return (startSquare == move.startSquare && endSquare == move.endSquare && promotionPiece == move.promotionPiece);
    }
    Move() : startSquare(0), endSquare(0), promotionPiece(Type::Empty){}
    Move(int start, int end);
    Move(int start, int end, Type promotion);
};
