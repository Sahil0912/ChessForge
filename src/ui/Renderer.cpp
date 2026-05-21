#include "Renderer.hpp"
#include <iostream>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 

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
    InitAudioDevice();
    moveSelf = LoadSound("assets/sounds/move-self.mp3");
    moveCheck = LoadSound("assets/sounds/move-check.mp3");
    capture = LoadSound("assets/sounds/capture.mp3");
    castle = LoadSound("assets/sounds/castle.mp3");
}

void Renderer::UnloadAssets(){
    for(int c = 0; c < 2; c++){
        for(int t = 1; t < 7; t++){
            UnloadTexture(_Pieces[c][t]);
        }
    }
    UnloadSound(moveSelf);
    UnloadSound(moveCheck);
    UnloadSound(capture);
    UnloadSound(castle);
    CloseAudioDevice();

}

void Renderer::Draw(Board& _Board){
    int state = 1;
    Draw(_Board, state);
}

void Renderer::Draw(Board& _Board, int &stateOfApp){

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    GuiSetStyle(DEFAULT, TEXT_SIZE, 40); 

// for menu state
    if(stateOfApp == 0){
        ClearBackground(Color{30, 30, 30, 255});

        const char* title = "CHESS FORGE";
        int fontSize = 80;
        DrawText(title, (w - MeasureText(title, fontSize))/2, h * 0.2, fontSize, WHITE);

        float btnWidth = 500;
        float btnHeight = 80;
        float startX = (w - btnWidth) / 2;
        float startY = h * 0.45;
        float gap = 15;

        if (GuiButton((Rectangle){startX, startY, btnWidth, btnHeight}, "Player vs Player")) {
            stateOfApp = 1; 
        }

        if (GuiButton((Rectangle){startX, startY + (btnHeight + gap), btnWidth, btnHeight}, "Player vs ChessForge")) {
            stateOfApp = 3;
        }

        if (GuiButton((Rectangle){startX, startY + (btnHeight + gap) * 2, btnWidth, btnHeight}, "Player vs Stockfish")) {
            stateOfApp = 2;
        }

        if (GuiButton((Rectangle){startX, startY + (btnHeight + gap) * 3, btnWidth, btnHeight}, "Quit Game")) {
            exit(0);
        }
        return;
    }

//for default pVp state
    ClearBackground(WHITE); 

    GameState state = _Board.GetState();

    int startFile = -1, startRank = -1, endFile = -1, endRank = -1;
    Color yellowTransparent = { 255, 255, 0, 100 };

    if (!_Board.history.empty()) {
        Move lastMove = _Board.history.back();
        
        startFile = lastMove.startSquare % 8;
        startRank = lastMove.startSquare / 8;
        endFile = lastMove.endSquare % 8;
        endRank = lastMove.endSquare / 8;
    }

    for(int file = 0; file < 8; file++){
        for(int row = 0; row < 8; row++){

            if(std::make_pair(startFile, startRank) != std::make_pair(file, row) && std::make_pair(endFile, endRank) != std::make_pair(file, row))
                DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, (file + row) % 2 ? BLUE : WHITE);
            else
                DrawRectangle(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, yellowTransparent);

            Piece _piece = _Board.GetPiece(row * 8 + file);
            if(_piece.color == _Board.GetTurn() && _piece.type == Type::King){
                Colors oppColor = (Colors)(1 - (int)_Board.GetTurn());
                if(_Board.isSquareAttacked(row * 8 + file, oppColor)){
                     DrawRectangleGradientV(file * _Tilesize, row * _Tilesize, _Tilesize, _Tilesize, Fade(RED, 0.0f), Fade(RED, 0.8f));
                     
                }
            }
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
        return;
    } 
    if(selectedSquare != -1){
        std::vector<Move> moves = _Board.GenerateMoves();
        for(auto &move : moves){
            if(move.startSquare == selectedSquare){
                int file = move.endSquare % 8;
                int rank = move.endSquare / 8;
                int center_x = file * _Tilesize + _Tilesize/2;
                int center_y = rank * _Tilesize + _Tilesize/2;
                if(_Board.GetPiece(move.endSquare).type == Type::Empty){
                    //a dot
                    DrawCircle(center_x, center_y, _Tilesize*0.15, Fade(DARKGRAY, 0.5f));
                }
                else{
                    float outerRadius = _Tilesize * 0.45f;
                    float innerRadius = _Tilesize * 0.40f; 
                    
                    DrawRing(
                        (Vector2){(float)center_x, (float)center_y}, 
                        innerRadius, outerRadius, 0.0f, 360.0f, 32, Fade(DARKGRAY, 0.5f)
                    );
                }
            }
        }
    }
    if(state != GameState::Playing){
        DrawRectangle(0, 0, _Tilesize * 8, _Tilesize * 8, Fade(BLACK, 0.7f));
        const char* message = "";
        switch (state)
        {
            case GameState::BlackWin:
                message = "Black Won!!!";
                break;
            
            case GameState::WhiteWin:
                message = "White Won!!!";
                break;
            
            case GameState::Draw:
                message = "Draw!!!";
                break;
            
            default:
                break;
        }
        int fontSize = 60;
        int textWidth = MeasureText(message, fontSize);
        int x = (_Tilesize * 8 - textWidth) / 2;
        int y = (_Tilesize * 8 - fontSize) / 2;

        DrawText(message, x, y, fontSize, WHITE);

        const char* restartMessage = "Press R to Restart";
        textWidth = MeasureText(restartMessage, 30);
        DrawText(restartMessage, (_Tilesize * 8 - textWidth)/2, y + 80, 30, LIGHTGRAY);

        const char* evalMessage = "Press E to Evaluate";
        textWidth = MeasureText(evalMessage, 30);
        DrawText(evalMessage, (_Tilesize * 8 - textWidth)/2, y + 120, 30, LIME);
    }
}

void Renderer::HandleInput(Board& _Board){
    int state = 1;
    HandleInput(_Board, state);
}

void Renderer::HandleInput(Board& _Board, int &stateOfApp){
    if(stateOfApp != 1 && stateOfApp != 2 && stateOfApp != 3) return;

    // in pVstockfish or pVForge and its engine turn
    if((stateOfApp == 2 || stateOfApp == 3) && _Board.GetTurn() == Colors::Black) {
        return; 
    }

// // for menu state
//     if(stateOfApp == 0){
//         if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
//             Rectangle pVp = {300, 450, 400, 80}; 
//             Rectangle pVstockfish = {300, 580, 400, 80};

//             Vector2 mouse = GetMousePosition();
            
//             bool hoverPvP = CheckCollisionPointRec(mouse, pVp);
//             bool hoverpVstockfish = CheckCollisionPointRec(mouse, pVstockfish);
            
//             if(hoverPvP){
//                 stateOfApp = 1;
                
//                 return;
//             }
//             else if(hoverpVstockfish){
//                 stateOfApp = 2;

//                 return;
//             }

//         }
//         return;
//     }
    

//for default pVp state
    if(_Board.GetState() != GameState::Playing){
        if(IsKeyPressed(KEY_R)){
            _Board.Initialize();
            stateOfApp = 0;
            isPromoting = false; 
        }
        return;
    }

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

            bool isCapture = (_Board.GetPiece(pendingMove.endSquare).type != Type::Empty);
            bool isCastle = (_Board.GetPiece(pendingMove.startSquare).type == Type::King && abs(pendingMove.startSquare - pendingMove.endSquare) == 2);

            _Board.MakeMove(pendingMove);
            PlaySounds(_Board, isCapture, isCastle);
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
                // auto findIterator = std::find(moves.begin(), moves.end(), currMove); // will no longer work as we can check castrling only with start and end
                auto findIterator = std::find_if(moves.begin(), moves.end(), [&](const Move& m) {
                    return m.startSquare == selectedSquare && m.endSquare == endSquare;
                });
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
                        pendingMove = *findIterator;
                    }
                    else{ // normal
                        Move currmove = *findIterator;
                        bool isCapture = (_Board.GetPiece(currmove.endSquare).type != Type::Empty);
                        bool isCastle = (_Board.GetPiece(currmove.startSquare).type == Type::King && abs(currmove.startSquare - currmove.endSquare) == 2);

                        _Board.MakeMove(currmove);
                        PlaySounds(_Board, isCapture, isCastle);
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

void Renderer::PlaySounds(Board& _Board, bool isCapture, bool isCastle){


    Colors turn = _Board.GetTurn();
    Colors oppColor = (Colors)(1 - (int)turn);
    bool isCheck = _Board.isSquareAttacked(_Board.findKing(turn), oppColor);

    if (isCheck) PlaySound(moveCheck);
    else if (isCastle) PlaySound(castle);
    else if (isCapture) PlaySound(capture);
    else PlaySound(moveSelf);
}

void Renderer::DrawEvalMode(){
    ClearBackground(Color{30, 30, 30, 255});

    // Replay board to evalMoveIndex
    Board replayBoard;
    replayBoard.Initialize();
    for(int i = 0; i < evalMoveIndex && i < (int)evalGameMoves.size(); i++){
        replayBoard.MakeMove(evalGameMoves[i]);
    }

    int evalTileSize = 75;

    int boardOffsetY = (1000 - 600) / 2; // center vertically

    // Draw board (left side, 600px)
    for(int file = 0; file < 8; file++){
        for(int row = 0; row < 8; row++){
            DrawRectangle(file * evalTileSize, boardOffsetY + row * evalTileSize, evalTileSize, evalTileSize,
                         (file + row) % 2 ? BLUE : WHITE);

            Piece _piece = replayBoard.GetPiece(row * 8 + file);
            if(_piece.type != Type::Empty) {
                DrawTextureEx(_Pieces[(int)_piece.color][(int)_piece.type], 
                             { (float)file * evalTileSize, (float)(boardOffsetY + row * evalTileSize) }, 
                             0.0f, (float)evalTileSize / 128.0f, WHITE);
            }
        }
    }

    // Highlight last move if evalMoveIndex > 0
    if(evalMoveIndex > 0 && evalMoveIndex <= (int)evalGameMoves.size()){
        Move lastMove = evalGameMoves[evalMoveIndex - 1];
        Color yellowTransparent = { 255, 255, 0, 100 };
        int sf = lastMove.startSquare % 8, sr = lastMove.startSquare / 8;
        int ef = lastMove.endSquare % 8, er = lastMove.endSquare / 8;
        DrawRectangle(sf * evalTileSize, boardOffsetY + sr * evalTileSize, evalTileSize, evalTileSize, yellowTransparent);
        DrawRectangle(ef * evalTileSize, boardOffsetY + er * evalTileSize, evalTileSize, evalTileSize, yellowTransparent);
    }

    if (selectedSquare != -1) {
        std::vector<Move> moves = replayBoard.GenerateMoves();
        for (auto &move : moves) {
            if (move.startSquare == selectedSquare) {
                int file = move.endSquare % 8;
                int rank = move.endSquare / 8;
                int center_x = file * evalTileSize + evalTileSize / 2;
                int center_y = boardOffsetY + rank * evalTileSize + evalTileSize / 2;
                
                if (replayBoard.GetPiece(move.endSquare).type == Type::Empty) {
                    DrawCircle(center_x, center_y, evalTileSize * 0.15, Fade(DARKGRAY, 0.5f));
                } else {
                    float outerRadius = evalTileSize * 0.45f;
                    float innerRadius = evalTileSize * 0.40f; 
                    DrawRing(
                        (Vector2){(float)center_x, (float)center_y}, 
                        innerRadius, outerRadius, 0.0f, 360.0f, 32, Fade(DARKGRAY, 0.5f)
                    );
                }
            }
        }
    }

    // === SIDEBAR (400px, starting at x=600) ===
    int sidebarX = 600;
    int sidebarW = 400;
    int screenH = 1000;

    // Sidebar background
    DrawRectangle(sidebarX, 0, sidebarW, screenH, Color{25, 25, 25, 255});
    DrawLine(sidebarX, 0, sidebarX, screenH, Color{60, 60, 60, 255});

    // Title
    const char* title = "POST-GAME ANALYSIS";
    int titleW = MeasureText(title, 22);
    DrawText(title, sidebarX + (sidebarW - titleW)/2, 15, 22, WHITE);
    DrawLine(sidebarX + 10, 45, sidebarX + sidebarW - 10, 45, Color{60, 60, 60, 255});

    // Current eval display
    int currentForge = 0, currentSF = 0;
    if(evalMoveIndex > 0 && evalMoveIndex <= (int)evalResults.size()){
        currentForge = evalResults[evalMoveIndex - 1].forgeEvalCp;
        currentSF = evalResults[evalMoveIndex - 1].stockfishEvalCp;
    }

    // Eval bar (vertical, left side of sidebar)
    int barX = sidebarX + 20;
    int barY = 60;
    int barW = 30;
    int barH = 300;

    // Clamp eval to [-1000, 1000] for bar display
    float sfClamp = (float)currentSF;
    if(sfClamp > 1000) sfClamp = 1000;
    if(sfClamp < -1000) sfClamp = -1000;
    // White portion: 0.5 + eval/2000 (0.5 = even, 1.0 = white winning)
    float whitePortion = 0.5f + sfClamp / 2000.0f;
    int whiteH = (int)(barH * whitePortion);
    int blackH = barH - whiteH;

    // Draw bar: black on top, white on bottom
    DrawRectangle(barX, barY, barW, blackH, Color{50, 50, 50, 255});
    DrawRectangle(barX, barY + blackH, barW, whiteH, Color{240, 240, 240, 255});
    DrawRectangleLines(barX, barY, barW, barH, Color{80, 80, 80, 255});

    // Eval numbers next to bar
    int evalTextX = barX + barW + 15;
    char sfBuf[64], forgeBuf[64];
    snprintf(sfBuf, sizeof(sfBuf), "Stockfish: %s%.2f",
             currentSF >= 0 ? "+" : "", currentSF / 100.0f);
    snprintf(forgeBuf, sizeof(forgeBuf), "Forge:     %s%.2f",
             currentForge >= 0 ? "+" : "", currentForge / 100.0f);

    DrawText(sfBuf, evalTextX, barY + 10, 20, Color{100, 200, 255, 255});
    DrawText(forgeBuf, evalTextX, barY + 40, 20, Color{255, 180, 80, 255});

    // Move counter
    char moveBuf[64];
    snprintf(moveBuf, sizeof(moveBuf), "Move %d of %d", evalMoveIndex, (int)evalGameMoves.size());
    DrawText(moveBuf, evalTextX, barY + 80, 18, LIGHTGRAY);

    // Separator
    DrawLine(sidebarX + 10, barY + barH + 20, sidebarX + sidebarW - 10, barY + barH + 20,
             Color{60, 60, 60, 255});

    // === Move list ===
    int listY = barY + barH + 30;
    int listH = screenH - listY - 50; // leave room for nav hint
    int rowH = 24;
    int maxVisible = listH / rowH;

    // Header
    DrawText("#", sidebarX + 15, listY, 16, GRAY);
    DrawText("Move", sidebarX + 45, listY, 16, GRAY);
    DrawText("Forge", sidebarX + 130, listY, 16, Color{255, 180, 80, 255});
    DrawText("SF", sidebarX + 230, listY, 16, Color{100, 200, 255, 255});
    listY += rowH + 4;
    int maxScroll = std::max(0, (int)evalResults.size() - maxVisible);

    // Safety clamp
    if(evalScrollStart > maxScroll) evalScrollStart = maxScroll;
    if(evalScrollStart < 0) evalScrollStart = 0;

    for(int i = evalScrollStart; i < (int)evalResults.size() && i < evalScrollStart + maxVisible; i++){
        int y = listY + (i - evalScrollStart) * rowH;
        bool isCurrent = (i == evalMoveIndex - 1);

        if(isCurrent){
            DrawRectangle(sidebarX + 5, y - 2, sidebarW - 10, rowH, Color{60, 60, 80, 255});
        }

        // Move number: "1." for white, "1..." for black
        int moveNum = (i / 2) + 1;
        char numBuf[16];
        if(i % 2 == 0) snprintf(numBuf, sizeof(numBuf), "%d.", moveNum);
        else snprintf(numBuf, sizeof(numBuf), "%d...", moveNum);

        Color textColor = isCurrent ? WHITE : LIGHTGRAY;
        DrawText(numBuf, sidebarX + 15, y, 16, textColor);
        DrawText(evalResults[i].uci.c_str(), sidebarX + 55, y, 16, textColor);

        char fBuf[16], sBuf[16];
        snprintf(fBuf, sizeof(fBuf), "%+.2f", evalResults[i].forgeEvalCp / 100.0f);
        snprintf(sBuf, sizeof(sBuf), "%+.2f", evalResults[i].stockfishEvalCp / 100.0f);

        // Color code: green if positive (good for white), red if negative
        Color forgeColor = evalResults[i].forgeEvalCp >= 0 ?
            Color{100, 220, 100, 255} : Color{220, 100, 100, 255};
        Color sfColor = evalResults[i].stockfishEvalCp >= 0 ?
            Color{100, 220, 100, 255} : Color{220, 100, 100, 255};

        DrawText(fBuf, sidebarX + 125, y, 16, forgeColor);
        DrawText(sBuf, sidebarX + 220, y, 16, sfColor);
    }

    // Draw visual scrollbar
    int scrollbarX = sidebarX + sidebarW - 15;
    int scrollbarY = listY;
    int scrollbarW = 10;
    int scrollbarH = listH - (rowH + 4);

    if ((int)evalResults.size() > maxVisible) {
        // Track
        DrawRectangle(scrollbarX, scrollbarY, scrollbarW, scrollbarH, Color{40, 40, 40, 255});
        // Thumb
        float contentRatio = (float)maxVisible / evalResults.size();
        int thumbH = std::max(20, (int)(scrollbarH * contentRatio));
        float scrollRatio = (float)evalScrollStart / maxScroll;
        int thumbY = scrollbarY + (int)(scrollRatio * (scrollbarH - thumbH));
        DrawRectangle(scrollbarX, thumbY, scrollbarW, thumbH, Color{120, 120, 120, 255});
    }

    // Navigation hint at bottom
    const char* navHint = "<- -> Navigate | R Exit";
    int navW = MeasureText(navHint, 18);
    DrawText(navHint, sidebarX + (sidebarW - navW)/2, screenH - 30, 18, GRAY);
}

void Renderer::HandleEvalInput(int &stateOfApp){
    bool moveChanged = false;
    if(IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)){
        if(evalMoveIndex < (int)evalGameMoves.size()){
            evalMoveIndex++;
            moveChanged = true;
        }
    }
    if(IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)){
        if(evalMoveIndex > 0){
            evalMoveIndex--;
            moveChanged = true;
        }
    }

    if(moveChanged && evalMoveIndex > 0) {
        int listY = 60 + 300 + 30; // barY + barH + 30
        int screenH = 1000;
        int listH = screenH - listY - 50;
        int maxVisible = listH / 24; // rowH
        
        if (evalMoveIndex - 1 < evalScrollStart) {
            evalScrollStart = std::max(0, evalMoveIndex - 1);
        } else if (evalMoveIndex - 1 >= evalScrollStart + maxVisible) {
            evalScrollStart = (evalMoveIndex - 1) - maxVisible + 1;
        }
    }
    if(IsKeyPressed(KEY_R)){
        evalMode = false;
        evalMoveIndex = 0;
        evalScrollStart = 0;
        evalScrollAccum = 0.0f;
        evalResults.clear();
        evalGameMoves.clear();
        stateOfApp = 0;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        evalScrollAccum -= wheel * 3.0f;
        int scrollDelta = (int)evalScrollAccum;
        if (scrollDelta != 0) {
            evalScrollStart += scrollDelta;
            evalScrollAccum -= scrollDelta;
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        int sidebarX = 600;
        
        if (mousePos.x <= sidebarX) {
            // Rebuild board state to validate moves
            Board replayBoard;
            replayBoard.Initialize();
            for(int i = 0; i < evalMoveIndex && i < (int)evalGameMoves.size(); i++){
                replayBoard.MakeMove(evalGameMoves[i]);
            }

            int evalTileSize = 75;
            int boardOffsetY = (1000 - 600) / 2;
            int file = mousePos.x / evalTileSize;
            int row = (mousePos.y - boardOffsetY) / evalTileSize;
            
            if (file >= 0 && file < 8 && row >= 0 && row < 8) {
                int clickedSquare = row * 8 + file;
                
                if (selectedSquare == -1) {
                    Piece p = replayBoard.GetPiece(clickedSquare);
                    if (p.type != Type::Empty && p.color == replayBoard.GetTurn()) {
                        selectedSquare = clickedSquare;
                    }
                } else {
                    std::vector<Move> moves = replayBoard.GenerateMoves();
                    bool moveMade = false;
                    for (auto &move : moves) {
                        if (move.startSquare == selectedSquare && move.endSquare == clickedSquare) {
                            // Auto-promote to Queen in eval mode for simplicity
                            if (move.promotionPiece != Type::Empty) {
                                move.promotionPiece = Type::Queen;
                            }
                            
                            // Truncate future history
                            if (evalMoveIndex < (int)evalGameMoves.size()) {
                                evalGameMoves.erase(evalGameMoves.begin() + evalMoveIndex, evalGameMoves.end());
                                evalResults.erase(evalResults.begin() + evalMoveIndex, evalResults.end());
                            }
                            
                            evalGameMoves.push_back(move);
                            evalMoveIndex++;
                            hasPendingEval = true;
                            moveMade = true;
                            break;
                        }
                    }
                    selectedSquare = -1; // deselect after clicking somewhere
                }
            } else {
                selectedSquare = -1; // clicked outside board bounds but in the 600px width area
            }
        } else {
            // Sidebar logic
            selectedSquare = -1; // clear selection if clicked sidebar
            int barY = 60;
            int barH = 300;
            int listY = barY + barH + 30;
            int screenH = 1000;
            int listH = screenH - listY - 50;
            int rowH = 24;
            int maxVisible = listH / rowH;
            int itemsStartY = listY + rowH + 4;
            int maxScroll = std::max(0, (int)evalResults.size() - maxVisible);
            
            // Check scrollbar click
            int scrollbarX = sidebarX + 400 - 15;
            int scrollbarY = itemsStartY;
            int scrollbarW = 10;
            int scrollbarH = listH - (rowH + 4);

            if ((int)evalResults.size() > maxVisible &&
                mousePos.x >= scrollbarX && mousePos.x <= scrollbarX + scrollbarW &&
                mousePos.y >= scrollbarY && mousePos.y <= scrollbarY + scrollbarH) {
                float clickRatio = (mousePos.y - scrollbarY) / (float)scrollbarH;
                evalScrollStart = (int)(clickRatio * maxScroll);
            } 
            else if (mousePos.y >= itemsStartY && mousePos.y < itemsStartY + maxVisible * rowH) {
                int clickedRow = (mousePos.y - itemsStartY) / rowH;
                int clickedIndex = evalScrollStart + clickedRow;
                
                if (clickedIndex >= 0 && clickedIndex < (int)evalResults.size()) {
                    evalMoveIndex = clickedIndex + 1;
                }
            }
        }
    }
}