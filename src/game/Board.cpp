#include "Board.hpp"
#include <random>

// Zobrist static members
uint64_t Board::zobristPieces[64][2][7];
uint64_t Board::zobristSide;
uint64_t Board::zobristCastling[4];
uint64_t Board::zobristEnPassant[8];
bool Board::zobristInitialized = false;

void Board::InitZobrist() {
    if (zobristInitialized) return;
    std::mt19937_64 rng(0xDEADBEEF42ULL);
    for (int sq = 0; sq < 64; sq++)
        for (int c = 0; c < 2; c++)
            for (int p = 0; p < 7; p++)
                zobristPieces[sq][c][p] = rng();
    zobristSide = rng();
    for (int i = 0; i < 4; i++) zobristCastling[i] = rng();
    for (int i = 0; i < 8; i++) zobristEnPassant[i] = rng();
    zobristInitialized = true;
}

void Board::ComputeHashFromScratch() {
    zobristHash = 0;
    for (int sq = 0; sq < 64; sq++) {
        if (squares[sq].type != Type::Empty) {
            zobristHash ^= zobristPieces[sq][(int)squares[sq].color][(int)squares[sq].type];
        }
    }
    if (turn == Colors::Black) zobristHash ^= zobristSide;
    if (whiteCastleKingSide)  zobristHash ^= zobristCastling[0];
    if (whiteCastleQueenSide) zobristHash ^= zobristCastling[1];
    if (blackCastleKingSide)  zobristHash ^= zobristCastling[2];
    if (blackCastleQueenSide) zobristHash ^= zobristCastling[3];
    if (enPassantSquare != -1) zobristHash ^= zobristEnPassant[enPassantSquare % 8];
}

uint64_t Board::GetHash() const { return zobristHash; }
const std::array<Piece, 64>& Board::GetSquares() const { return squares; }
void Board::SetState(GameState state) { gameState = state; }
int Board::GetEnPassantSquare() const { return enPassantSquare; }

bool Board::IsThreefoldRepetition() const {
    int count = 0;
    for (const auto& h : hashHistory) {
        if (h == zobristHash) {
            count++;
            if (count >= 3) return true;
        }
    }
    return false;
}

GameState Board::CheckGameState() {
    // Threefold repetition
    if (IsThreefoldRepetition()) {
        gameState = GameState::Draw;
        return gameState;
    }

    std::vector<Move> moves = GenerateMoves();
    if (moves.empty()) {
        Colors oppTurn = (Colors)(1 - (int)turn);
        if (isSquareAttacked(findKing(turn), oppTurn)) {
            if (turn == Colors::Black) gameState = GameState::WhiteWin;
            else gameState = GameState::BlackWin;
        } else {
            gameState = GameState::Draw;
        }
    }
    return gameState;
}

void Board::Initialize(){
    turn = Colors::White;
    gameState = GameState::Playing;
    for (int i = 16; i < 48; i++)
    {
        squares[i].type = Type::Empty;
        squares[i].color = Colors::None;
    }
    for (int i = 0; i < 8; i++)
    {
        squares[i].color = Colors::Black;
    }
    for (int i = 56; i < 64; i++)
    {
        squares[i].color = Colors::White;
    }
    for (int i = 8; i < 16; i++)
    {
        squares[i].type = Type::Pawn;
        squares[i].color = Colors::Black;
    }
    for (int i = 48; i < 56; i++)
    {
        squares[i].type = Type::Pawn;
        squares[i].color = Colors::White;
    }
    squares[0].type = squares[7].type = Type::Rook; 
    squares[1].type = squares[6].type = Type::Knight;
    squares[2].type = squares[5].type = Type::Bishop;
    squares[3].type = Type::Queen;
    squares[4].type = Type::King;

    squares[56].type = squares[63].type = Type::Rook; 
    squares[57].type = squares[62].type = Type::Knight;
    squares[58].type = squares[61].type = Type::Bishop;
    squares[59].type = Type::Queen;
    squares[60].type = Type::King;

    whiteCastleKingSide = whiteCastleQueenSide = blackCastleKingSide = blackCastleQueenSide = true;
    enPassantSquare = -1;
    history.clear();
    hashHistory.clear();
    ComputeHashFromScratch();
    hashHistory.push_back(zobristHash);
}

Piece Board::GetPiece(int index) const {
    return squares[index];
}

Colors Board::GetTurn() const {
    return turn;
}

GameState Board::GetState() const{
    return gameState;
}

bool Board::isSquareAttacked(int square, Colors color) { //attacked by this color or not
    //to see if this square is attacking which piece
    Colors oppColor = (Colors)(1 - (int)color);
    //bishop or queen

    std::vector<std::pair<int, bool>> offsets{{9, true}, {-9, true}, {7, true}, {-7, true}};
    std::vector<Move> moves;
    GenerateSlidingMoves(square, oppColor, offsets, moves);
    for(auto &move : moves){
        if(squares[move.endSquare].type == Type::Bishop || squares[move.endSquare].type == Type::Queen){
            return true;
        }
    }

    //rook or queen

    moves.clear();
    offsets = std::vector<std::pair<int, bool>>{{8, true}, {-8, true}, {1, false}, {-1, false}};
    GenerateSlidingMoves(square, oppColor, offsets, moves);
    for(auto &move : moves){
        if(squares[move.endSquare].type == Type::Rook || squares[move.endSquare].type == Type::Queen){
            return true;
        }
    }

    moves.clear();
    GenerateKnightMoves(square, oppColor, moves);
    for(auto &move : moves){
        if(squares[move.endSquare].type == Type::Knight){
            return true;
        }
    }

    moves.clear();
    GeneratePawnMoves(square, oppColor, moves);
    for(auto &move : moves){
        if(squares[move.endSquare].type == Type::Pawn){
            return true;
        }
    }

    moves.clear();
    GenerateKingMoves(square, oppColor, moves);
    for(auto &move : moves){
        if(squares[move.endSquare].type == Type::King){
            return true;
        }
    }

    return false;
}

void Board::GeneratePawnMoves(int startSquare, Colors& color, std::vector<Move> &moves){
    // Helper: add a move, generating all 4 promotions if on the last rank
    auto addPawnMove = [&](int from, int to) {
        bool isPromotion = (color == Colors::Black && to >= 56) ||
                           (color == Colors::White && to < 8);
        if (isPromotion) {
            moves.push_back(Move(from, to, Type::Queen));
            moves.push_back(Move(from, to, Type::Rook));
            moves.push_back(Move(from, to, Type::Bishop));
            moves.push_back(Move(from, to, Type::Knight));
        } else {
            moves.push_back(Move(from, to));
        }
    };

    if(color == Colors::Black){
        // Have to move in positive direction
        if(startSquare < 16){
            // first move (double push — can't promote from rank 7 double push)
            if(squares[startSquare + 16].type == Type::Empty && squares[startSquare + 8].type == Type::Empty){
                Move move(startSquare, startSquare + 16);
                moves.push_back(move);
            }
        }
        // single push
        if(startSquare + 8 < 64 && squares[startSquare + 8].type == Type::Empty){
            addPawnMove(startSquare, startSquare + 8);
        }
        // captures
        if((startSquare + 1) % 8){
            if(startSquare + 9 < 64 && (enPassantSquare == startSquare + 9 || (squares[startSquare + 9].type != Type::Empty && squares[startSquare + 9].color == Colors::White))){
                addPawnMove(startSquare, startSquare + 9);
            }
        }
        if(startSquare % 8){
            if(startSquare + 7 < 64 && (enPassantSquare == startSquare + 7 || (squares[startSquare + 7].type != Type::Empty && squares[startSquare + 7].color == Colors::White))){
                addPawnMove(startSquare, startSquare + 7);
            }
        }
    }
    else if(color == Colors::White){
        // Have to move in negative direction
        if(startSquare > 47){
            // first move (double push)
            if(squares[startSquare - 16].type == Type::Empty && squares[startSquare - 8].type == Type::Empty){
                Move move(startSquare, startSquare - 16);
                moves.push_back(move);
            }
        }
        // single push
        if(startSquare - 8 >= 0 && squares[startSquare - 8].type == Type::Empty){
            addPawnMove(startSquare, startSquare - 8);
        }
        // captures
        if((startSquare + 1) % 8){
            if(startSquare - 7 >= 0 && (enPassantSquare == startSquare - 7 || (squares[startSquare - 7].type != Type::Empty && squares[startSquare - 7].color == Colors::Black))){
                addPawnMove(startSquare, startSquare - 7);
            }
        }
        if(startSquare % 8){
            if(startSquare - 9 >= 0 && (enPassantSquare == startSquare - 9 || (squares[startSquare - 9].type != Type::Empty && squares[startSquare - 9].color == Colors::Black))){
                addPawnMove(startSquare, startSquare - 9);
            }
        }
    }
}



void Board::GenerateKnightMoves(int startSquare, Colors& color, std::vector<Move> &moves){
    // + 17 + 15 - 17 - 15
    if((startSquare + 1) % 8 && startSquare + 17 < 64 && (squares[startSquare + 17].type == Type::Empty || squares[startSquare + 17].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare + 17);
        moves.push_back(move);
    }
    if(startSquare % 8 && startSquare + 15 < 64 && (squares[startSquare + 15].type == Type::Empty || squares[startSquare + 15].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare + 15);
        moves.push_back(move);
    }
    if(startSquare % 8 && startSquare - 17 >= 0 && (squares[startSquare - 17].type == Type::Empty || squares[startSquare - 17].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare - 17);
        moves.push_back(move);
    }
    if((startSquare + 1) % 8 && startSquare - 15 >= 0 && (squares[startSquare - 15].type == Type::Empty || squares[startSquare - 15].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare - 15);
        moves.push_back(move);
    }

    // + 10 + 6 - 10 - 6
    if((startSquare + 1) % 8 && (startSquare + 2) % 8 && startSquare + 10 < 64 && (squares[startSquare + 10].type == Type::Empty || squares[startSquare + 10].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare + 10);
        moves.push_back(move);
    }
    if(startSquare % 8 && (startSquare - 1) % 8 && startSquare + 6 < 64 && (squares[startSquare + 6].type == Type::Empty || squares[startSquare + 6].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare + 6);
        moves.push_back(move);
    }
    if(startSquare % 8 && (startSquare - 1) % 8 && startSquare - 10 >= 0 && (squares[startSquare - 10].type == Type::Empty || squares[startSquare - 10].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare - 10);
        moves.push_back(move);
    }
    if((startSquare + 1) % 8 && (startSquare + 2) % 8 && startSquare - 6 >= 0 && (squares[startSquare - 6].type == Type::Empty || squares[startSquare - 6].color == (Colors)(1 - (int)color))){
        Move move(startSquare, startSquare - 6);
        moves.push_back(move);
    }

}

bool valid(int square, std::pair<int,bool> &offset){
    if(offset.second){ //meaning to go to next row
        if(offset.first > 0){
            if(square >= 56) return false;
            if((offset.first) <= 7 - square % 8 || offset.first > 15 - square % 8) return false;
            return true;
        }
        else{
            if(square <= 7) return false;
            if((-1) * (offset.first) <= square % 8 || (-1) * (offset.first) > 8 + square % 8) return false;
            return true;
        }
    }
    else{ // same row
        if(offset.first > 0){
            if((offset.first) > 7 - square % 8) return false;
            return true;
        }
        else{
            if((-1) * (offset.first) > square % 8) return false;
            return true;
        }
    }
}



void Board::GenerateSlidingMoves(int startSquare, Colors& color, std::vector<std::pair<int, bool>> &offsets, std::vector<Move> &moves){
    for(auto &offset : offsets){
        int square = startSquare;
        while(valid(square, offset)){
            square += offset.first;
            if(squares[square].type != Type::Empty && squares[square].color == (Colors)(1 - (int)color)){
                Move move(startSquare, square);
                moves.push_back(move);
                break;
            }
            if(squares[square].type != Type::Empty){
                break;
            }
            Move move(startSquare, square);
            moves.push_back(move);
        }
    }
}

void Board::GenerateBishopMoves(int startSquare, Colors& color, std::vector<Move> &moves){
    std::vector<std::pair<int, bool>> offsets{{9, true}, {-9, true}, {7, true}, {-7, true}};
    GenerateSlidingMoves(startSquare, color, offsets, moves);
}
void Board::GenerateRookMoves(int startSquare, Colors& color, std::vector<Move> &moves){
    std::vector<std::pair<int, bool>> offsets{{8, true}, {-8, true}, {1, false}, {-1, false}};
    GenerateSlidingMoves(startSquare, color, offsets, moves);
}
void Board::GenerateQueenMoves(int startSquare, Colors& color, std::vector<Move> &moves){
    std::vector<std::pair<int, bool>> offsets{{9, true}, {-9, true}, {7, true}, {-7, true}, {8, true}, {-8, true}, {1, false}, {-1, false}};
    GenerateSlidingMoves(startSquare, color, offsets, moves);
}

void Board::GenerateKingMoves(int startSquare, Colors& color, std::vector<Move> &moves){
    std::vector<std::pair<int, bool>> offsets{{9, true}, {-9, true}, {7, true}, {-7, true}, {8, true}, {-8, true}, {1, false}, {-1, false}};
    for(auto &offset : offsets){
        int square = startSquare;
        if(valid(square, offset)){
            square += offset.first;
            if(squares[square].type != Type::Empty && squares[square].color == (Colors)(1 - (int)color)){
                Move move(startSquare, square);
                moves.push_back(move);
                continue;
            }
            if(squares[square].type != Type::Empty){
                continue;
            }
            Move move(startSquare, square);
            moves.push_back(move);
        }
    }
}

void Board::GeneratePieceMoves(int startSquare, Type& type, Colors& color, std::vector<Move> &moves){
    if(type == Type::Pawn) GeneratePawnMoves(startSquare, color, moves);
    else if(type == Type::Knight) GenerateKnightMoves(startSquare, color, moves);
    else if(type == Type::Bishop) GenerateBishopMoves(startSquare, color, moves);
    else if(type == Type::Rook) GenerateRookMoves(startSquare, color, moves);
    else if(type == Type::Queen) GenerateQueenMoves(startSquare, color, moves);
    else if(type == Type::King) GenerateKingMoves(startSquare, color, moves);
}

int Board::findKing(Colors color){
    for (int pos = 0; pos < 64; pos++)
    {
        if(squares[pos].type == Type::King && squares[pos].color == color) return pos;
    }
    return -1;
}

std::vector<Move> Board::GenerateMoves(){
    std::vector<Move> moves;
    for(int i = 0; i < 64; i++){
        if(squares[i].color == turn){
            GeneratePieceMoves(i, squares[i].type, squares[i].color, moves);
        }
    }
    Colors oppTurn = (Colors)(1 - (int)turn);
    
    std::vector<Move> legalMoves;
    for(auto &move : moves){
        Piece currStartPiece = squares[move.startSquare];
        Piece currEndPiece = squares[move.endSquare];

        // checking for pawn enPassant in diff case (doing here because say the king is in check with the previous pawn which can be taken using enPassant)
        //so if I check the logic here the move will validate and if I would have checked the logic only in the makeMove part it would have never reached there in the firt place
        if(enPassantSquare != -1 && (currStartPiece.type == Type::Pawn && currEndPiece.type == Type::Empty && (move.endSquare - move.startSquare) % 8)){
            //setting up for checking
            squares[move.endSquare] = squares[move.startSquare];
            squares[move.startSquare] = {Colors::None, Type::Empty};
            if(turn == Colors::White){
                squares[move.endSquare + 8] = {Colors::None, Type::Empty};
            }
            else if(turn == Colors::Black){
                squares[move.endSquare - 8] = {Colors::None, Type::Empty};
            }

            //checking
            int kingPos = findKing(turn);
        
            if(!isSquareAttacked(kingPos, oppTurn)){
                legalMoves.push_back(move);
            }

            //unsetting
            squares[move.endSquare] = currEndPiece;
            squares[move.startSquare] = currStartPiece;

            if(turn == Colors::White){
                squares[move.endSquare + 8] = {Colors::Black, Type::Pawn};
            }
            else if(turn == Colors::Black){
                squares[move.endSquare - 8] = {Colors::White, Type::Pawn};
            }
            continue;
        }

        //setting up for checking
        squares[move.endSquare] = squares[move.startSquare];
        squares[move.startSquare] = {Colors::None, Type::Empty};

        //checking
        int kingPos = findKing(turn);
        
        if(!isSquareAttacked(kingPos, oppTurn)){
            legalMoves.push_back(move);
        }

        //unset
        squares[move.endSquare] = currEndPiece;
        squares[move.startSquare] = currStartPiece;

    }
    if(turn == Colors::White){
        if(whiteCastleKingSide){
            if(squares[61].type == Type::Empty && squares[62].type == Type::Empty && !isSquareAttacked(60, oppTurn) && !isSquareAttacked(61, oppTurn) && !isSquareAttacked(62, oppTurn)){
                Move move(60, 62, 63, 61, true);
                legalMoves.push_back(move);
            }
        }
        if(whiteCastleQueenSide){
            if(squares[57].type == Type::Empty && squares[58].type == Type::Empty && squares[59].type == Type::Empty && !isSquareAttacked(60, oppTurn) && !isSquareAttacked(59, oppTurn) && !isSquareAttacked(58, oppTurn)){
                Move move(60, 58, 56, 59, true);
                legalMoves.push_back(move);
            }
        }
    }
    else if(turn == Colors::Black){
        if(blackCastleKingSide){
            if(squares[5].type == Type::Empty && squares[6].type == Type::Empty && !isSquareAttacked(4, oppTurn) && !isSquareAttacked(5, oppTurn) && !isSquareAttacked(6, oppTurn)){
                Move move(4, 6, 7, 5, true);
                legalMoves.push_back(move);
            }
        }
        if(blackCastleQueenSide){
            if(squares[1].type == Type::Empty && squares[2].type == Type::Empty && squares[3].type == Type::Empty && !isSquareAttacked(4, oppTurn) && !isSquareAttacked(3, oppTurn) && !isSquareAttacked(2, oppTurn)){
                Move move(4, 2, 0, 3, true);
                legalMoves.push_back(move);
            }
        }
    }
    return legalMoves;
}

UndoInfo Board::MakeMove(Move move){
    // Save undo info
    UndoInfo undo;
    undo.capturedPiece = squares[move.endSquare];
    undo.enPassantSquare = enPassantSquare;
    undo.whiteCastleKingSide = whiteCastleKingSide;
    undo.whiteCastleQueenSide = whiteCastleQueenSide;
    undo.blackCastleKingSide = blackCastleKingSide;
    undo.blackCastleQueenSide = blackCastleQueenSide;
    undo.zobristHash = zobristHash;

    int startSquare = move.startSquare;
    int endSquare = move.endSquare;

    // Remove old en passant from hash
    if(enPassantSquare != -1) zobristHash ^= zobristEnPassant[enPassantSquare % 8];
    enPassantSquare = -1;

    // Remove old castling from hash
    if(whiteCastleKingSide)  zobristHash ^= zobristCastling[0];
    if(whiteCastleQueenSide) zobristHash ^= zobristCastling[1];
    if(blackCastleKingSide)  zobristHash ^= zobristCastling[2];
    if(blackCastleQueenSide) zobristHash ^= zobristCastling[3];

    // Update castling flags
    if(whiteCastleKingSide){
        if(startSquare == 63 || endSquare == 63 || (squares[startSquare].color == Colors::White && squares[startSquare].type == Type::King))
            whiteCastleKingSide = false;
    }
    if(whiteCastleQueenSide){
        if(startSquare == 56 || endSquare == 56 || (squares[startSquare].color == Colors::White && squares[startSquare].type == Type::King))
            whiteCastleQueenSide = false;
    }
    if(blackCastleKingSide){
        if(startSquare == 7 || endSquare == 7 || (squares[startSquare].color == Colors::Black && squares[startSquare].type == Type::King))
            blackCastleKingSide = false;
    }
    if(blackCastleQueenSide){
        if(startSquare == 0 || endSquare == 0 || (squares[startSquare].color == Colors::Black && squares[startSquare].type == Type::King))
            blackCastleQueenSide = false;
    }

    // Add new castling to hash
    if(whiteCastleKingSide)  zobristHash ^= zobristCastling[0];
    if(whiteCastleQueenSide) zobristHash ^= zobristCastling[1];
    if(blackCastleKingSide)  zobristHash ^= zobristCastling[2];
    if(blackCastleQueenSide) zobristHash ^= zobristCastling[3];

    if(move.isCastling){
        // XOR out king and rook from old squares
        zobristHash ^= zobristPieces[startSquare][(int)squares[startSquare].color][(int)squares[startSquare].type];
        zobristHash ^= zobristPieces[move.oldRookSquare][(int)squares[move.oldRookSquare].color][(int)squares[move.oldRookSquare].type];

        squares[endSquare] = squares[startSquare];
        squares[move.newRookSquare] = squares[move.oldRookSquare];
        squares[startSquare] = {Colors::None, Type::Empty};
        squares[move.oldRookSquare] = {Colors::None, Type::Empty};

        // XOR in king and rook at new squares
        zobristHash ^= zobristPieces[endSquare][(int)squares[endSquare].color][(int)squares[endSquare].type];
        zobristHash ^= zobristPieces[move.newRookSquare][(int)squares[move.newRookSquare].color][(int)squares[move.newRookSquare].type];
    }
    else{
        // XOR out moving piece from start
        zobristHash ^= zobristPieces[startSquare][(int)squares[startSquare].color][(int)squares[startSquare].type];
        // XOR out captured piece (if any)
        if(squares[endSquare].type != Type::Empty)
            zobristHash ^= zobristPieces[endSquare][(int)squares[endSquare].color][(int)squares[endSquare].type];

        if(squares[startSquare].type == Type::Pawn){
            if(turn == Colors::Black){
                if(startSquare < 16 && endSquare == startSquare + 16)
                    enPassantSquare = startSquare + 8;
                // En passant capture
                if((endSquare == startSquare + 7 || endSquare == startSquare + 9) && undo.capturedPiece.type == Type::Empty){
                    zobristHash ^= zobristPieces[endSquare - 8][(int)squares[endSquare - 8].color][(int)squares[endSquare - 8].type];
                    squares[endSquare - 8] = {Colors::None, Type::Empty};
                }
            }
            else if(turn == Colors::White){
                if(startSquare >= 48 && endSquare == startSquare - 16)
                    enPassantSquare = startSquare - 8;
                // En passant capture
                if((endSquare == startSquare - 7 || endSquare == startSquare - 9) && undo.capturedPiece.type == Type::Empty){
                    zobristHash ^= zobristPieces[endSquare + 8][(int)squares[endSquare + 8].color][(int)squares[endSquare + 8].type];
                    squares[endSquare + 8] = {Colors::None, Type::Empty};
                }
            }
        }

        if(move.promotionPiece != Type::Empty){
            squares[endSquare].type = move.promotionPiece;
        } else {
            squares[endSquare].type = squares[startSquare].type;
        }
        squares[endSquare].color = squares[startSquare].color;
        squares[startSquare] = {Colors::None, Type::Empty};

        // XOR in piece at destination
        zobristHash ^= zobristPieces[endSquare][(int)squares[endSquare].color][(int)squares[endSquare].type];
    }

    // Add new en passant to hash
    if(enPassantSquare != -1) zobristHash ^= zobristEnPassant[enPassantSquare % 8];

    // Flip side
    zobristHash ^= zobristSide;
    turn = (Colors)(1 - (int)turn);
    history.push_back(move);
    hashHistory.push_back(zobristHash);
    return undo;
}

void Board::UnmakeMove(const Move& move, const UndoInfo& undo){
    // Pop from history
    if(!history.empty()) history.pop_back();
    if(!hashHistory.empty()) hashHistory.pop_back();

    // Flip turn back
    turn = (Colors)(1 - (int)turn);

    int startSquare = move.startSquare;
    int endSquare = move.endSquare;

    if(move.isCastling){
        // Move king back
        squares[startSquare] = squares[endSquare];
        squares[endSquare] = {Colors::None, Type::Empty};
        // Move rook back
        squares[move.oldRookSquare] = squares[move.newRookSquare];
        squares[move.newRookSquare] = {Colors::None, Type::Empty};
    }
    else{
        // Detect en passant purely from move/undo info (NOT from board state)
        // En passant = pawn moved diagonally to en passant square, no piece was captured on that square
        bool wasPawnMove = (move.promotionPiece != Type::Empty) ||
                           (squares[endSquare].type == Type::Pawn);
        int fileDiff = (endSquare % 8) - (startSquare % 8);
        bool wasDiagonal = (fileDiff != 0);
        bool wasEnPassant = wasPawnMove && wasDiagonal &&
                            undo.enPassantSquare != -1 &&
                            endSquare == undo.enPassantSquare &&
                            undo.capturedPiece.type == Type::Empty;

        if(wasEnPassant){
            // Restore the captured pawn
            if(turn == Colors::White){
                squares[endSquare + 8] = {Colors::Black, Type::Pawn};
            } else {
                squares[endSquare - 8] = {Colors::White, Type::Pawn};
            }
        }

        // Move piece back (undo promotion)
        if(move.promotionPiece != Type::Empty){
            squares[startSquare] = {turn, Type::Pawn};
        } else {
            squares[startSquare] = squares[endSquare];
        }
        // Restore captured piece
        squares[endSquare] = undo.capturedPiece;
    }

    // Restore state
    enPassantSquare = undo.enPassantSquare;
    whiteCastleKingSide = undo.whiteCastleKingSide;
    whiteCastleQueenSide = undo.whiteCastleQueenSide;
    blackCastleKingSide = undo.blackCastleKingSide;
    blackCastleQueenSide = undo.blackCastleQueenSide;
    zobristHash = undo.zobristHash;
}


std::string Board::IndexToSquare(int index){
    int file = index % 8;
    int row = 8 - index / 8;
    std::string square = "";
    square += (char)(file + 'a');
    square += (char)(row + '0'); 
    return square;
}

int Board::SquareToIndex(std::string square){
    int file = (square[0] - 'a');
    int row = 8 - (square[1] - '0');
    int index = row * 8 + file;
    return index;
}

Move Board::UciToMove(std::string uci, Board& _Board){
    //will be square1 + square2
    std::string square1 = ""; 
    square1 += uci[0];
    square1 += uci[1];

    std::string square2 = ""; 
    square2 += uci[2];
    square2 += uci[3];

    int index1 = SquareToIndex(square1);
    int index2 = SquareToIndex(square2);
    Move move(index1, index2);

    if(uci.length() == 5){
        if(uci[4] == 'q') move.promotionPiece = Type::Queen;
        else if(uci[4] == 'r') move.promotionPiece = Type::Rook;
        else if(uci[4] == 'b') move.promotionPiece = Type::Bishop;
        else if(uci[4] == 'n') move.promotionPiece = Type::Knight; 
    }
    if(_Board.squares[move.startSquare].type == Type::King && abs(move.startSquare - move.endSquare) == 2){
        move.isCastling = true;
        if(index1 < index2){
            move.oldRookSquare = index2 + 1;
            move.newRookSquare = index1 + 1;
        }
        else{
            move.oldRookSquare = index2 - 2;
            move.newRookSquare = index1 - 1;
        }
    }
    return move;    
}

std::string Board::MoveToUci(Move move){
    if(move.startSquare == 0 && move.endSquare == 0 && move.promotionPiece == Type::Empty){
        return "(none)";
    }
    std::string uci = IndexToSquare(move.startSquare) + IndexToSquare(move.endSquare);    
    if(move.promotionPiece != Type::Empty){
        if(move.promotionPiece == Type::Queen) uci += 'q';
        else if(move.promotionPiece == Type::Rook) uci += 'r';
        else if(move.promotionPiece == Type::Bishop) uci += 'b';
        else if(move.promotionPiece == Type::Knight) uci += 'n';
    }
    
    return uci;
}
