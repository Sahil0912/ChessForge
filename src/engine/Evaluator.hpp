#pragma once

#include "../game/Board.hpp"

class Evaluator{
public:
    // returns score in centipawns from side-to-move perspective
    static int Evaluate(const Board& board);

private:
    // PeSTO material values
    static constexpr int MG_VALUE[7] = {0, 82, 337, 365, 477, 1025, 0};
    static constexpr int EG_VALUE[7] = {0, 94, 281, 297, 512, 936, 0};

    static constexpr int PHASE_INC[7] = {0, 0, 1, 1, 2, 4, 0};

    // piece-square tables (index 0 = a8)
    // white uses directly, black mirrors with sq^56
    static const int MG_PST[7][64];
    static const int EG_PST[7][64];
};
