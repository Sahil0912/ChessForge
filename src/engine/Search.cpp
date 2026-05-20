#include "Search.hpp"
#include "Evaluator.hpp"
#include <algorithm>
#include <cstring>

Search::Search() : tt(TT_SIZE){
    std::memset(killers, 0, sizeof(killers));
}

bool Search::IsTimeUp(){
    if(timeUp) return true;
    if((nodesSearched & 2047) == 0){ // check every 2048 nodes
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - searchStart).count();
        if(elapsed >= timeLimitMs){
            timeUp = true;
            return true;
        }
    }
    return false;
}

// mate scores need adjustment when storing/retrieving from TT
static int ScoreToTT(int score, int ply){
    if(score >= 49000 - 64) return score + ply;
    if(score <= -49000 + 64) return score - ply;
    return score;
}

static int ScoreFromTT(int score, int ply){
    if(score >= 49000 - 64) return score - ply;
    if(score <= -49000 + 64) return score + ply;
    return score;
}

void Search::StoreTT(uint64_t key, int depth, int score, TTFlag flag, const Move& bestMove){
    int index = key % TT_SIZE;
    TTEntry& entry = tt[index];
    if(entry.key == 0 || depth >= entry.depth || entry.key == key){
        entry.key = key;
        entry.depth = depth;
        entry.score = ScoreToTT(score, 0);
        entry.flag = flag;
        entry.bestMove = bestMove;
    }
}

TTEntry* Search::ProbeTT(uint64_t key){
    int index = key % TT_SIZE;
    TTEntry& entry = tt[index];
    if(entry.key == key) return &entry;
    return nullptr;
}

int Search::MvvLvaScore(const Move& move, const Board& board) const{
    static constexpr int victimScore[7]   = {0, 100, 300, 300, 500, 900, 10000};
    static constexpr int attackerScore[7] = {0,   5,   4,   3,   2,   1,     0};

    int victim = (int)board.GetPiece(move.endSquare).type;
    int attacker = (int)board.GetPiece(move.startSquare).type;
    return victimScore[victim] * 10 + attackerScore[attacker];
}

void Search::OrderMoves(std::vector<Move>& moves, const Board& board,
                        const Move& ttMove, int ply){
    std::vector<int> scores(moves.size(), 0);

    for(int i = 0; i < (int)moves.size(); i++){
        // TT move highest priority
        if(moves[i].startSquare == ttMove.startSquare &&
           moves[i].endSquare == ttMove.endSquare &&
           moves[i].promotionPiece == ttMove.promotionPiece){
            scores[i] = 100000;
            continue;
        }

        // captures (MVV-LVA)
        if(board.GetPiece(moves[i].endSquare).type != Type::Empty){
            scores[i] = 10000 + MvvLvaScore(moves[i], board);
            continue;
        }

        // promotions
        if(moves[i].promotionPiece != Type::Empty){
            scores[i] = 9000 + (int)moves[i].promotionPiece;
            continue;
        }

        // killer moves (non-captures that caused cutoffs at this ply)
        if(ply >= 0 && ply < MAX_PLY){
            if(moves[i].startSquare == killers[ply][0].startSquare &&
               moves[i].endSquare == killers[ply][0].endSquare){
                scores[i] = 8000;
                continue;
            }
            if(moves[i].startSquare == killers[ply][1].startSquare &&
               moves[i].endSquare == killers[ply][1].endSquare){
                scores[i] = 7000;
                continue;
            }
        }
    }

    // selection sort
    for(int i = 0; i < (int)moves.size(); i++){
        int best = i;
        for(int j = i + 1; j < (int)moves.size(); j++){
            if(scores[j] > scores[best]) best = j;
        }
        if(best != i){
            std::swap(moves[i], moves[best]);
            std::swap(scores[i], scores[best]);
        }
    }
}

Move Search::FindBestMove(Board& board, int timeLimit){
    searchStart = std::chrono::steady_clock::now();
    timeLimitMs = timeLimit;
    timeUp = false;
    nodesSearched = 0;
    depthReached = 0;
    std::memset(killers, 0, sizeof(killers));

    Move bestMove;
    auto moves = board.GenerateMoves();
    if(moves.empty()) return bestMove;
    if(moves.size() == 1) return moves[0];

    bestMove = moves[0]; // fallback

    // iterative deepening
    for(int depth = 1; depth <= MAX_PLY; depth++){
        int alpha = -INF;
        int beta = INF;
        Move iterBestMove = moves[0];
        int bestScore = -INF;

        auto currentMoves = board.GenerateMoves();

        // grab TT move for ordering
        Move ttMove;
        TTEntry* ttEntry = ProbeTT(board.GetHash());
        if(ttEntry) ttMove = ttEntry->bestMove;
        OrderMoves(currentMoves, board, ttMove, 0);

        for(auto &move : currentMoves){
            UndoInfo undo = board.MakeMove(move);
            int score = -Negamax(board, depth - 1, -beta, -alpha, 1);
            board.UnmakeMove(move, undo);

            if(timeUp) break;

            if(score > bestScore){
                bestScore = score;
                iterBestMove = move;
            }
            if(score > alpha){
                alpha = score;
            }
        }

        if(timeUp) break;

        bestMove = iterBestMove;
        depthReached = depth;

        // found a mate, stop searching
        if(bestScore >= MATE_SCORE - MAX_PLY || bestScore <= -MATE_SCORE + MAX_PLY){
            break;
        }
    }

    return bestMove;
}

int Search::Negamax(Board& board, int depth, int alpha, int beta, int ply){
    if(IsTimeUp()) return 0;

    nodesSearched++;

    // TT probe
    uint64_t hash = board.GetHash();
    TTEntry* ttEntry = ProbeTT(hash);
    Move ttMove;
    if(ttEntry){
        ttMove = ttEntry->bestMove;
        if(ttEntry->depth >= depth){
            int ttScore = ScoreFromTT(ttEntry->score, ply);
            if(ttEntry->flag == TT_EXACT) return ttScore;
            if(ttEntry->flag == TT_ALPHA && ttScore <= alpha) return alpha;
            if(ttEntry->flag == TT_BETA && ttScore >= beta) return beta;
        }
    }

    // leaf node -> drop into quiescence
    if(depth <= 0){
        return Quiescence(board, alpha, beta, ply);
    }

    auto moves = board.GenerateMoves();

    // checkmate or stalemate
    if(moves.empty()){
        Colors oppTurn = (Colors)(1 - (int)board.GetTurn());
        if(board.isSquareAttacked(board.findKing(board.GetTurn()), oppTurn)){
            return -MATE_SCORE + ply; // prefer shorter mates
        }
        return 0; // stalemate
    }

    OrderMoves(moves, board, ttMove, ply);

    int bestScore = -INF;
    Move bestMove = moves[0];
    TTFlag flag = TT_ALPHA;

    for(auto &move : moves){
        UndoInfo undo = board.MakeMove(move);
        int score = -Negamax(board, depth - 1, -beta, -alpha, ply + 1);
        board.UnmakeMove(move, undo);

        if(timeUp) return 0;

        if(score > bestScore){
            bestScore = score;
            bestMove = move;
        }
        if(score > alpha){
            alpha = score;
            flag = TT_EXACT;
        }
        if(alpha >= beta){
            flag = TT_BETA;
            // store killer (non-capture that caused cutoff)
            if(board.GetPiece(move.endSquare).type == Type::Empty && ply < MAX_PLY){
                killers[ply][1] = killers[ply][0];
                killers[ply][0] = move;
            }
            break;
        }
    }

    StoreTT(hash, depth, bestScore, flag, bestMove);
    return bestScore;
}

int Search::Quiescence(Board& board, int alpha, int beta, int ply){
    if(IsTimeUp()) return 0;

    nodesSearched++;

    Colors oppTurn = (Colors)(1 - (int)board.GetTurn());
    bool inCheck = board.isSquareAttacked(board.findKing(board.GetTurn()), oppTurn);

    // stand pat (only if NOT in check)
    if(!inCheck){
        int standPat = Evaluator::Evaluate(board);
        if(standPat >= beta) return beta;
        if(standPat > alpha) alpha = standPat;
    }

    auto moves = board.GenerateMoves();

    if(moves.empty()){
        if(inCheck) return -MATE_SCORE + ply;
        return 0; // stalemate
    }

    // in check -> search ALL moves (must escape)
    if(inCheck){
        Move noMove;
        OrderMoves(moves, board, noMove, ply);

        int bestScore = -INF;
        for(auto &move : moves){
            UndoInfo undo = board.MakeMove(move);
            int score = -Quiescence(board, -beta, -alpha, ply + 1);
            board.UnmakeMove(move, undo);

            if(timeUp) return 0;

            if(score > bestScore) bestScore = score;
            if(score >= beta) return beta;
            if(score > alpha) alpha = score;
        }
        return bestScore;
    }

    // not in check -> only captures and promotions
    std::vector<Move> captures;
    for(auto &move : moves){
        if(board.GetPiece(move.endSquare).type != Type::Empty ||
           move.promotionPiece != Type::Empty){
            captures.push_back(move);
        }
        // en passant: pawn diagonal to empty square
        else if(board.GetPiece(move.startSquare).type == Type::Pawn){
            int fileDiff = (move.endSquare % 8) - (move.startSquare % 8);
            if(fileDiff != 0){
                captures.push_back(move);
            }
        }
    }

    Move noMove;
    OrderMoves(captures, board, noMove, ply);

    for(auto &move : captures){
        UndoInfo undo = board.MakeMove(move);
        int score = -Quiescence(board, -beta, -alpha, ply + 1);
        board.UnmakeMove(move, undo);

        if(timeUp) return 0;

        if(score >= beta) return beta;
        if(score > alpha) alpha = score;
    }

    return alpha;
}
