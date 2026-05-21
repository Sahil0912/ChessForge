#pragma once
#include <raylib.h>
#include "../game/Board.hpp"
#include <algorithm>
#include <vector>
#include <string>

struct EvalData {
    std::string uci;       // e.g. "e2e4"
    int forgeEvalCp;       // centipawns, White's perspective
    int stockfishEvalCp;   // centipawns, White's perspective
    std::string forgeBestMove;
    std::string stockfishBestMove;
};

class Renderer{
    private : 
        const int _Tilesize = 125;
        Texture2D _Pieces[2][7]; //color and which piece (0 is white and 1 is black)
        // mapping same as Type and Color
        int selectedSquare;
        bool isPromoting;
        Move pendingMove; //for promotion thing
    public :
        Sound moveSelf;
        Sound moveCheck;
        Sound capture;
        Sound castle;

        // Eval mode state
        bool evalMode = false;
        int evalMoveIndex = 0;
        int evalScrollStart = 0;
        float evalScrollAccum = 0.0f;
        bool hasPendingEval = false;
        std::vector<EvalData> evalResults;
        std::vector<Move> evalGameMoves; // stored copy of the game's move history

        Renderer() : selectedSquare(-1), isPromoting(false) {}
        ~Renderer() = default;
        void LoadAssets();          
        void Draw(Board& _Board);
        void Draw(Board& _Board, int &stateofApp);
        void DrawEvalMode();
        void HandleInput(Board& _Board);
        void HandleInput(Board& _Board, int &stateOfApp);
        void HandleEvalInput(int &stateOfApp);
        void UnloadAssets();
        void PlaySounds(Board& _Board, bool isCapture, bool isCastle);
};