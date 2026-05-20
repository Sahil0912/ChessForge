#include <raylib.h>
#include "game/Board.hpp"
#include "game/Piece.hpp"
#include "ui/Renderer.hpp"
#include "engine/Engine.hpp"
#include <string>
#include <memory>

int main(){

    Board::InitZobrist();

    const int ScreenSize = 1000;
    InitWindow(ScreenSize, ScreenSize, "Chess Forge");
    Board _Board;
    _Board.Initialize();
    Renderer _Renderer;
    _Renderer.LoadAssets();
    SetTargetFPS(60);
    int stateOfApp = 0; // 0 - Menu, 1 - pVp, 2 - pVStockfish, 3 - pVForge

    // Engine instances
    StockfishEngine stockfish;
    ForgeEngine forgeEngine;
    std::vector<std::string> moveHistory; //for stockfish
    bool stockfishStarted = false;
    int lastDepth = 0;
    int lastNodes = 0;

    while (!WindowShouldClose())
    {
        //crash prevent for restart
        if(IsKeyPressed(KEY_R)){
            _Board.Initialize();
            moveHistory.clear();
            stockfishStarted = false;
            stockfish = StockfishEngine();
            lastDepth = 0;
            lastNodes = 0;
            stateOfApp = 0;
        }

        // Start stockfish if needed
        if(stateOfApp == 2 && !stockfishStarted){
            stockfishStarted = true;
            moveHistory.clear();
            stockfish.Start();
        }

        Colors turnBefore = _Board.GetTurn();
        _Renderer.HandleInput(_Board, stateOfApp);
        Colors turnAfter = _Board.GetTurn();

        // After player move, detect game over
        if (turnAfter != turnBefore) {
            _Board.CheckGameState();
        }

        //drawing
        BeginDrawing();
        _Renderer.Draw(_Board, stateOfApp);

        // Show engine stats in ForgeEngine mode
        if (stateOfApp == 3 && lastDepth > 0) {
            std::string depthStr = "Depth: " + std::to_string(lastDepth);
            std::string nodesStr = "Nodes: " + std::to_string(lastNodes);
            DrawText(depthStr.c_str(), 10, 10, 20, LIME);
            DrawText(nodesStr.c_str(), 10, 35, 20, LIME);
        }

        EndDrawing();

        // Stockfish engine turn
        if(stateOfApp == 2 && turnAfter != turnBefore && turnAfter == Colors::Black){
            moveHistory.push_back(Board::MoveToUci(_Board.history.back()));
            if(_Board.GetState() == GameState::Playing){
                Move engineMove = stockfish.GetBestMove(_Board, 1000);
                bool isCapture = (_Board.GetPiece(engineMove.endSquare).type != Type::Empty);
                bool isCastle = engineMove.isCastling;

                _Board.MakeMove(engineMove);
                _Board.CheckGameState();
                moveHistory.push_back(Board::MoveToUci(engineMove));

                Colors turn = _Board.GetTurn();
                Colors oppColor = (Colors)(1 - (int)turn);
                bool isCheck = _Board.isSquareAttacked(_Board.findKing(turn), oppColor);

                if (isCheck) PlaySound(_Renderer.moveCheck);
                else if (isCastle) PlaySound(_Renderer.castle);
                else if (isCapture) PlaySound(_Renderer.capture);
                else PlaySound(_Renderer.moveSelf);

                BeginDrawing();
                _Renderer.Draw(_Board, stateOfApp);
                EndDrawing();
            }
        }

        // ForgeEngine turn
        if(stateOfApp == 3 && turnAfter != turnBefore && turnAfter == Colors::Black){
            if(_Board.GetState() == GameState::Playing){
                Move engineMove = forgeEngine.GetBestMove(_Board, 2000);
                bool isCapture = (_Board.GetPiece(engineMove.endSquare).type != Type::Empty);
                bool isCastle = engineMove.isCastling;

                _Board.MakeMove(engineMove);
                _Board.CheckGameState();

                lastDepth = forgeEngine.GetLastSearchDepth();
                lastNodes = forgeEngine.GetNodesSearched();

                Colors turn = _Board.GetTurn();
                Colors oppColor = (Colors)(1 - (int)turn);
                bool isCheck = _Board.isSquareAttacked(_Board.findKing(turn), oppColor);

                if (isCheck) PlaySound(_Renderer.moveCheck);
                else if (isCastle) PlaySound(_Renderer.castle);
                else if (isCapture) PlaySound(_Renderer.capture);
                else PlaySound(_Renderer.moveSelf);

                BeginDrawing();
                _Renderer.Draw(_Board, stateOfApp);
                // Show search stats overlay
                std::string depthStr = "Depth: " + std::to_string(lastDepth);
                std::string nodesStr = "Nodes: " + std::to_string(lastNodes);
                DrawText(depthStr.c_str(), 10, 10, 20, LIME);
                DrawText(nodesStr.c_str(), 10, 35, 20, LIME);
                EndDrawing();
            }
        }
    }
    
    _Renderer.UnloadAssets();
    CloseWindow();
    return 0;
}