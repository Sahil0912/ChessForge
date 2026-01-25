#include "Move.hpp"

Move::Move(int start, int end){
    startSquare = start;
    endSquare = end;
    promotionPiece = Type::Empty;
    oldRookSquare = -1;
    newRookSquare = -1;
    isCastling = false;
}

Move::Move(int start, int end, Type promotion){
    startSquare = start;
    endSquare = end;
    promotionPiece = promotion;
    oldRookSquare = -1;
    newRookSquare = -1;
    isCastling = false;
}
Move::Move(int start, int end, int oldRook, int newRook, bool isCastle){
    startSquare = start;
    endSquare = end;
    oldRookSquare = oldRook;
    newRookSquare = newRook;
    isCastling = isCastle;
    promotionPiece = Type::Empty;
}
