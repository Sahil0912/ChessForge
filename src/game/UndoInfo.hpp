#pragma once

#include "Piece.hpp"
#include <cstdint>

// Captures all board state destroyed by MakeMove, enabling UnmakeMove
struct UndoInfo {
    Piece capturedPiece;           // What was on endSquare before the move
    int   enPassantSquare;         // Previous en passant target square
    bool  whiteCastleKingSide;
    bool  whiteCastleQueenSide;
    bool  blackCastleKingSide;
    bool  blackCastleQueenSide;
    uint64_t zobristHash;          // Hash before the move
};
