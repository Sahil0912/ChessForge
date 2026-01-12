#pragma once
#include "Piece.h"
#include <array>

class Board
{
private:
    std::array<Piece, 64> squares; // 0 ->a8 and 63 -> h1
    Colors turn;

public:
    Board() = default;
    ~Board() = default;

    void Initialize();
    Piece GetPiece(int index) const;
};


