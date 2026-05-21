#pragma once

#include "../game/Board.hpp"
#include "../game/Move.hpp"
#include <vector>
#include <cstdint>
#include <chrono>

enum TTFlag : uint8_t {
    TT_EXACT = 0,
    TT_ALPHA = 1, // upper bound
    TT_BETA  = 2  // lower bound (beta cutoff)
};

struct TTEntry {
    uint64_t key = 0;
    int      depth = 0;
    int      score = 0;
    TTFlag   flag = TT_EXACT;
    Move     bestMove;
};

class Search{
public:
    Search();

    Move FindBestMove(Board& board, int timeLimitMs);
    int  GetNodesSearched() const { return nodesSearched; }
    int  GetDepthReached() const { return depthReached; }
    int  GetLastBestScore() const { return lastBestScore; }

private:
    int lastBestScore = 0;
    int Negamax(Board& board, int depth, int alpha, int beta, int ply);
    int Quiescence(Board& board, int alpha, int beta, int ply);

    void OrderMoves(std::vector<Move>& moves, const Board& board, const Move& ttMove, int ply);
    int  MvvLvaScore(const Move& move, const Board& board) const;

    static constexpr int TT_SIZE = 1 << 20;
    std::vector<TTEntry> tt;
    void    StoreTT(uint64_t key, int depth, int score, TTFlag flag, const Move& bestMove);
    TTEntry* ProbeTT(uint64_t key);

    static constexpr int MAX_PLY = 64;
    Move killers[MAX_PLY][2]; // 2 per ply

    std::chrono::steady_clock::time_point searchStart;
    int timeLimitMs = 1000;
    bool timeUp = false;
    bool IsTimeUp();

    int nodesSearched = 0;
    int depthReached = 0;

    static constexpr int INF = 50000;
    static constexpr int MATE_SCORE = 49000;
};
