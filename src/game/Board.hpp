#pragma once
#include "Piece.hpp"
#include "Move.hpp"
#include <array>
#include <vector>

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
    void GeneratePawnMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateKnightMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateBishopMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateRookMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateQueenMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GenerateKingMoves(int startSquare, Colors& color, std::vector<Move> &moves);
    void GeneratePieceMoves(int startSquare, Type& type, Colors& color, std::vector<Move> &moves);
    void GenerateSlidingMoves(int startSquare, Colors& color, std::vector<std::pair<int, bool>> &offsets, std::vector<Move> &moves);
    
    int findKing(Colors& color);

public:
    Board() = default;
    ~Board() = default;

    void Initialize();
    GameState GetState() const;
    Piece GetPiece(int index) const;
    Colors GetTurn() const;
    std::vector<Move> GenerateMoves();
    bool isSquareAttacked(int square, Colors& color);
    void MakeMove(Move move);
};


