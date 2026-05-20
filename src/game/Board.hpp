#pragma once
#include "Piece.hpp"
#include "Move.hpp"
#include "UndoInfo.hpp"
#include <array>
#include <vector>
#include <string>
#include <cstdint>

enum class GameState{
    Playing, WhiteWin, BlackWin, Draw
};

class Board
{
private:
    std::array<Piece, 64> squares; // 0 ->a8 and 63 -> h1
    Colors turn = Colors::White;
    GameState gameState = GameState::Playing;
    bool whiteCastleKingSide = true, whiteCastleQueenSide = true, blackCastleKingSide = true, blackCastleQueenSide = true; 
    int enPassantSquare = -1;

    // Zobrist hashing
    uint64_t zobristHash = 0;
    static uint64_t zobristPieces[64][2][7]; // [square][color][pieceType]
    static uint64_t zobristSide;
    static uint64_t zobristCastling[4];      // WK, WQ, BK, BQ
    static uint64_t zobristEnPassant[8];     // per file
    static bool zobristInitialized;

    void GeneratePawnMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateKnightMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateBishopMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateRookMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateQueenMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateKingMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GeneratePieceMoves(int startSquare, Type& type, Colors& color, std::vector<Move> &moves);
    void GenerateSlidingMoves(int startSquare, Colors& color, std::vector<std::pair<int, bool>> &offsets, std::vector<Move> &moves);
    
    void ComputeHashFromScratch();

public:
    Board() = default;
    ~Board() = default;
    int findKing(Colors color);
    std::vector <Move> history;
    void Initialize();
    GameState GetState() const;
    void SetState(GameState state);
    GameState CheckGameState(); // generates moves, detects checkmate/stalemate
    Piece GetPiece(int index) const;
    const std::array<Piece, 64>& GetSquares() const;
    Colors GetTurn() const;
    int GetEnPassantSquare() const;
    std::vector<Move> GenerateMoves();
    bool isSquareAttacked(int square, Colors color);
    UndoInfo MakeMove(Move move);
    void UnmakeMove(const Move& move, const UndoInfo& undo);
    uint64_t GetHash() const;

    static void InitZobrist();
    static std::string IndexToSquare(int index);
    static int SquareToIndex(std::string square);
    static Move UciToMove(std::string uci, Board &_Board);
    static std::string MoveToUci(Move move);
};
