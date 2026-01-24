#include "Renderer.hpp"
#include <iostream>

void Renderer::LoadAssets(){
    _Pieces[(int)Colors::White][(int)Type::Pawn] = LoadTexture("assets/white-pawn.png");
    _Pieces[(int)Colors::White][(int)Type::Bishop] = LoadTexture("assets/white-bishop.png");
    _Pieces[(int)Colors::White][(int)Type::Knight] = LoadTexture("assets/white-knight.png");
    _Pieces[(int)Colors::White][(int)Type::Rook] = LoadTexture("assets/white-rook.png");
    _Pieces[(int)Colors::White][(int)Type::Queen] = LoadTexture("assets/white-queen.png");
    _Pieces[(int)Colors::White][(int)Type::King] = LoadTexture("assets/white-king.png");

    _Pieces[(int)Colors::Black][(int)Type::Pawn] = LoadTexture("assets/black-pawn.png");
    _Pieces[(int)Colors::Black][(int)Type::Bishop] = LoadTexture("assets/black-bishop.png");
    _Pieces[(int)Colors::Black][(int)Type::Knight] = LoadTexture("assets/black-knight.png");
    _Pieces[(int)Colors::Black][(int)Type::Rook] = LoadTexture("assets/black-rook.png");
    _Pieces[(int)Colors::Black][(int)Type::Queen] = LoadTexture("assets/black-queen.png");
    _Pieces[(int)Colors::Black][(int)Type::King] = LoadTexture("assets/black-king.png");

}
void Renderer::UnloadAssets(){
    for(int c = 0; c < 2; c++){
        for(int t = 1; t < 7; t++){
            UnloadTexture(_Pieces[c][t]);
        }
    }
    
}
void Renderer::Draw(const Board& _Board){
    for(int file = 0; file < 8; file++){
        for(int row = 0; row < 8; row++){
            DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, (file + row) % 2 ? BLUE : WHITE);
            Piece _piece = _Board.GetPiece(row * 8 + file);
            if(_piece.type != Type::Empty)
                DrawTexture(_Pieces[(int)_piece.color][(int)_piece.type], file * _Tilesize, row * _Tilesize, WHITE);
        }
    }
    
    //menu for promotion (reference taken from chess.com)
    if(isPromoting){
        int endSquare = pendingMove.endSquare;
        //sequence - Queen, knight, rook, Bishop
        Colors currColor = _Board.GetTurn();
        int offset = (currColor == Colors::Black ? -1 : 1);
        int file = endSquare % 8;
        int row = endSquare / 8;

        DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, GREEN);
        DrawTexture(_Pieces[(int)currColor][(int)Type::Queen], file * _Tilesize, row * _Tilesize, WHITE);

        row += offset;

        DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, GREEN);
        DrawTexture(_Pieces[(int)currColor][(int)Type::Knight], file * _Tilesize, row * _Tilesize, WHITE);

        row += offset;

        DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, GREEN);
        DrawTexture(_Pieces[(int)currColor][(int)Type::Rook], file * _Tilesize, row * _Tilesize, WHITE);

        row += offset;

        DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, GREEN);
        DrawTexture(_Pieces[(int)currColor][(int)Type::Bishop], file * _Tilesize, row * _Tilesize, WHITE);

    } 
}

void Renderer::HandleInput(Board& _Board){
    if(isPromoting){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){ //got promoted to piece what
            Vector2 mousePos = GetMousePosition();
            int x_coor_rect = mousePos.x / _Tilesize;
            int y_coor_rect = mousePos.y / _Tilesize;

            // logging
            std::cout << x_coor_rect << " " << y_coor_rect << std::endl;
            if(x_coor_rect != pendingMove.endSquare % 8){
                isPromoting = false;
                selectedSquare = -1;
                return;
            }
            
            //I only have to use the row number so y_coor_rect
            if(y_coor_rect == 7 || y_coor_rect == 0) pendingMove.promotionPiece = Type::Queen;
            else if(y_coor_rect == 6 || y_coor_rect == 1) pendingMove.promotionPiece = Type::Knight;
            else if(y_coor_rect == 5 || y_coor_rect == 2) pendingMove.promotionPiece = Type::Rook;
            else if(y_coor_rect == 4 || y_coor_rect == 3) pendingMove.promotionPiece = Type::Bishop;

            _Board.MakeMove(pendingMove);
            isPromoting = false;
        }
        return;
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        //Piece to move

        Vector2 mousePos = GetMousePosition();
        int x_coor_rect = mousePos.x / _Tilesize;
        int y_coor_rect = mousePos.y / _Tilesize;

        // logging
        std::cout << x_coor_rect << " " << y_coor_rect << std::endl;

        if(selectedSquare != -1){
            // logging
            std::cout << selectedSquare << std::endl;


            const Piece currPiece = _Board.GetPiece(selectedSquare);
            if(currPiece.color == _Board.GetTurn()){
                // good to go
                int endSquare = y_coor_rect * 8 + x_coor_rect;
                std::vector<Move> moves = _Board.GenerateMoves();
                Move currMove(selectedSquare, endSquare);
                auto findIterator = std::find(moves.begin(), moves.end(), currMove);
                if(findIterator == moves.end()){
                    const Piece endPiece = _Board.GetPiece(endSquare);
                    if(endPiece.color == currPiece.color){
                        selectedSquare = endSquare;
                    }
                    else {
                        selectedSquare = -1;
                    }
                }
                else{
                    if(currPiece.type == Type::Pawn && (endSquare < 8 || endSquare >= 56)){ //see if promoting or not
                        isPromoting = true;
                        pendingMove = currMove;
                    }
                    else{ // normal
                        _Board.MakeMove(currMove);
                    }
                    selectedSquare = -1;
                }
            }   
            else{ // already checking in outer else
                selectedSquare = -1;
            }
        }
        else {
            selectedSquare = y_coor_rect * 8 + x_coor_rect;
            const Piece currPiece = _Board.GetPiece(selectedSquare);
            if(currPiece.color != _Board.GetTurn()){
                selectedSquare = -1;
            }
        }
    }
    
}