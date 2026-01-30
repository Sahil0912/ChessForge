#include <raylib.h>
#include "game/Board.hpp"
#include "game/Piece.hpp"
#include "ui/Renderer.hpp"
#include "engine/Engine.hpp"


int main(){

    const int ScreenSize = 1000;
    InitWindow(ScreenSize, ScreenSize, "Chess");
    Board _Board;
    _Board.Initialize();
    Renderer _Renderer;
    _Renderer.LoadAssets();
    SetTargetFPS(60);
    int stateOfApp = 0; // 0 - Menu, 1 - pVp, 2 - pVstockfish 
    Engine stockfish;
    std::vector<std::string> moveHistory; //for engine
    bool engineOn = false;
    while (!WindowShouldClose())
    {
        if(stateOfApp == 2 && !engineOn){
            engineOn = true;
            moveHistory.clear();
            stockfish.Start();
        }
        Colors turnBefore = _Board.GetTurn();
        _Renderer.HandleInput(_Board, stateOfApp);
        Colors turnAfter = _Board.GetTurn();

        //drawing the current move;
        BeginDrawing();
        _Renderer.Draw(_Board, stateOfApp);
        EndDrawing();
        if(stateOfApp == 2 && turnAfter != turnBefore && turnAfter == Colors::Black){
            //stockfish turn
            moveHistory.push_back(Board::MoveToUci(_Board.history.back())); //getting user last move
            if(_Board.GetState() == GameState::Playing){
                std::string command = "position startpos moves";
                for(auto &str : moveHistory) command += " " + str;
                
                stockfish.WriteInteract(command);
                stockfish.WriteInteract("go movetime 1000"); //wait for 1sec
                std::string bestMove = stockfish.GetBestMove();
                if(!bestMove.empty()){
                    Move engineMove = Board::UciToMove(bestMove);
                    _Board.MakeMove(engineMove);
                    moveHistory.push_back(bestMove);

                    //curr engine move draw
                    BeginDrawing();
                    _Renderer.Draw(_Board, stateOfApp);
                    EndDrawing();
                }

            }      
            
        }
        
    }
    
    _Renderer.UnloadAssets();
    CloseWindow();
    return 0;
}