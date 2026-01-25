#pragma once

#include "Piece.hpp"

struct Move
{
    int startSquare;
    int endSquare;
    Type promotionPiece;
    bool isCastling;
    int oldRookSquare;
    int newRookSquare;
    bool operator==(const Move &move) const{
        return (startSquare == move.startSquare && endSquare == move.endSquare && promotionPiece == move.promotionPiece && isCastling == move.isCastling && oldRookSquare == move.oldRookSquare && newRookSquare == move.newRookSquare);
    }
    Move() : startSquare(0), endSquare(0), promotionPiece(Type::Empty), isCastling(false), oldRookSquare(-1), newRookSquare(-1){}
    Move(int start, int end);
    Move(int start, int end, Type promotion);
    Move(int start, int end, int oldRook, int newRook, bool isCastle);
};
