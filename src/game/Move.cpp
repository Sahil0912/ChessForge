#include "Move.hpp"

Move::Move(int start, int end){
    startSquare = start;
    endSquare = end;
    promotionPiece = Type::Empty;
}

Move::Move(int start, int end, Type promotion){
    startSquare = start;
    endSquare = end;
    promotionPiece = promotion;
}
