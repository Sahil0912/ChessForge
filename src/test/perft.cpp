#include "../game/Board.hpp"
#include <iostream>
#include <chrono>
#include <cstdint>

// count all leaf nodes at a given depth
uint64_t Perft(Board& board, int depth){
    if(depth == 0) return 1;

    auto moves = board.GenerateMoves();
    uint64_t nodes = 0;

    for(auto &move : moves){
        UndoInfo undo = board.MakeMove(move);
        nodes += Perft(board, depth - 1);
        board.UnmakeMove(move, undo);
    }

    return nodes;
}

// per-move node counts (for debugging which move is wrong)
void Divide(Board& board, int depth){
    auto moves = board.GenerateMoves();
    uint64_t total = 0;

    for(auto &move : moves){
        UndoInfo undo = board.MakeMove(move);
        uint64_t nodes = Perft(board, depth - 1);
        board.UnmakeMove(move, undo);
        total += nodes;

        std::string moveStr = Board::IndexToSquare(move.startSquare) +
                              Board::IndexToSquare(move.endSquare);
        if(move.promotionPiece != Type::Empty){
            const char promo[] = {' ', ' ', 'n', 'b', 'r', 'q', ' '};
            moveStr += promo[(int)move.promotionPiece];
        }
        std::cout << moveStr << ": " << nodes << std::endl;
    }

    std::cout << "\nTotal: " << total << std::endl;
}

int main(){
    Board::InitZobrist();
    Board board;
    board.Initialize();

    // known correct perft values for starting position
    struct PerftTest {
        int depth;
        uint64_t expected;
    };

    PerftTest tests[] = {
        {1, 20},
        {2, 400},
        {3, 8902},
        {4, 197281},
        {5, 4865609},
    };

    std::cout << "=== ChessForge Perft Test ===" << std::endl;
    std::cout << "Starting position" << std::endl << std::endl;

    bool allPassed = true;

    for(auto &test : tests){
        auto start = std::chrono::steady_clock::now();
        uint64_t result = Perft(board, test.depth);
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        bool passed = (result == test.expected);
        if(!passed) allPassed = false;

        std::cout << "Depth " << test.depth << ": " << result
                  << " (expected " << test.expected << ") "
                  << (passed ? "PASS" : "FAIL")
                  << " [" << ms << "ms]" << std::endl;

        if(!passed && test.depth <= 3){
            std::cout << "  Divide at depth " << test.depth << ":" << std::endl;
            Divide(board, test.depth);
        }
    }

    std::cout << std::endl;
    if(allPassed){
        std::cout << "ALL TESTS PASSED" << std::endl;
    }
    else{
        std::cout << "SOME TESTS FAILED - move generation has bugs!" << std::endl;
    }

    return 0;
}
