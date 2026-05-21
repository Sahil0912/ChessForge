#include <raylib.h>
#include "game/Board.hpp"
#include "game/Piece.hpp"
#include "ui/Renderer.hpp"
#include "engine/Engine.hpp"
#include "engine/Evaluator.hpp"
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
    int stateOfApp = 0; // 0 - Menu, 1 - pVp, 2 - pVStockfish, 3 - pVForge, 4 - EvalMode

    // Engine instances
    StockfishEngine stockfish;
    ForgeEngine forgeEngine;
    std::vector<std::string> moveHistory; //for stockfish
    bool stockfishStarted = false;
    int lastDepth = 0;
    int lastNodes = 0;

    while (!WindowShouldClose())
    {
        if (stateOfApp == -1) break;

        // Eval mode
        if(stateOfApp == 4){
            _Renderer.HandleEvalInput(stateOfApp);

            if(_Renderer.hasPendingEval){
                _Renderer.hasPendingEval = false;

                // Show mini-loading screen for the dynamic eval
                BeginDrawing();
                _Renderer.DrawEvalMode();
                int tw = MeasureText("Analyzing Move...", 20);
                DrawText("Analyzing Move...", 600 + (400 - tw)/2, 30, 20, YELLOW);
                EndDrawing();

                // Build state up to current
                std::vector<std::string> uciMoves;
                Board tempBoard;
                tempBoard.Initialize();
                for(int i = 0; i < _Renderer.evalMoveIndex; i++){
                    uciMoves.push_back(Board::MoveToUci(_Renderer.evalGameMoves[i]));
                    tempBoard.MakeMove(_Renderer.evalGameMoves[i]);
                }
                
                // Forge Eval (50ms search)
                Move forgeBestMoveObj = forgeEngine.GetBestMove(tempBoard, 50);
                int forgeEval = forgeEngine.GetLastBestScore();
                if(tempBoard.GetTurn() == Colors::Black) forgeEval = -forgeEval;
                
                // Stockfish Eval (Depth 10)
                StockfishEngine sfTemp;
                sfTemp.Start();
                auto sfResult = sfTemp.EvaluatePosition(uciMoves, 10);
                int sfEval = sfResult.first;
                if(tempBoard.GetTurn() == Colors::Black) sfEval = -sfEval;

                EvalData ed;
                ed.uci = uciMoves.back();
                ed.forgeEvalCp = forgeEval;
                ed.stockfishEvalCp = sfEval;
                ed.forgeBestMove = Board::MoveToUci(forgeBestMoveObj);
                ed.stockfishBestMove = sfResult.second;
                _Renderer.evalResults.push_back(ed);
            }

            BeginDrawing();
            _Renderer.DrawEvalMode();
            EndDrawing();
            continue;
        }

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

        // Enter evaluation mode after game over
        if(IsKeyPressed(KEY_E) && _Board.GetState() != GameState::Playing){
            // Show loading screen
            BeginDrawing();
            ClearBackground(Color{30, 30, 30, 255});
            const char* loadMsg = "Analyzing with Stockfish...";
            int tw = MeasureText(loadMsg, 40);
            DrawText(loadMsg, (1000 - tw)/2, 450, 40, LIME);
            const char* subMsg = "This may take a moment";
            int sw = MeasureText(subMsg, 24);
            DrawText(subMsg, (1000 - sw)/2, 510, 24, LIGHTGRAY);
            EndDrawing();

            // Store game moves
            std::vector<Move> gameMoves = _Board.history;
            _Renderer.evalGameMoves = gameMoves;
            _Renderer.evalResults.clear();

            // Start Stockfish for analysis
            StockfishEngine sfAnalyzer;
            sfAnalyzer.Start();

            // Replay game and evaluate each position after each move
            Board analysisBoard;
            analysisBoard.Initialize();
            std::vector<std::string> uciMoves;

            for(size_t i = 0; i < gameMoves.size(); i++){
                std::string uci = Board::MoveToUci(gameMoves[i]);
                uciMoves.push_back(uci);

                // Determine whose turn it WAS (before the move)
                Colors sideThatMoved = analysisBoard.GetTurn();
                analysisBoard.MakeMove(gameMoves[i]);

                // ForgeEngine search eval (from side-to-move perspective)
                Move forgeBestMoveObj = forgeEngine.GetBestMove(analysisBoard, 50); // 50ms search per move
                int forgeEval = forgeEngine.GetLastBestScore();
                // Convert to White's perspective
                // After the move, GetTurn() returns the OTHER side
                if(analysisBoard.GetTurn() == Colors::Black){
                    // White just moved, eval is from Black's perspective → negate
                    forgeEval = -forgeEval;
                }
                // else: Black just moved, eval is from White's perspective → keep

                // Stockfish eval
                auto sfResult = sfAnalyzer.EvaluatePosition(uciMoves, 10);
                int sfEval = sfResult.first;
                // Same conversion: SF returns from side-to-move perspective
                if(analysisBoard.GetTurn() == Colors::Black){
                    sfEval = -sfEval;
                }

                EvalData ed;
                ed.uci = uci;
                ed.forgeEvalCp = forgeEval;
                ed.stockfishEvalCp = sfEval;
                ed.forgeBestMove = Board::MoveToUci(forgeBestMoveObj);
                ed.stockfishBestMove = sfResult.second;
                _Renderer.evalResults.push_back(ed);
            }

            // Enter eval mode
            _Renderer.evalMode = true;
            _Renderer.evalMoveIndex = (int)gameMoves.size(); // start at final position
            stateOfApp = 4;
            continue;
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