#include "Board.hpp"

void Board::Initialize(){
    turn = Colors::White;
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
    gameState = GameState::Playing;
    enPassantSquare = -1;
    history.clear();
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

bool Board::isSquareAttacked(int square, Colors& color) { //attacked by this color or not
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
    if(color == Colors::Black){
        // Have to move in positive direction
        if(startSquare < 16){
            // first move 
            if(squares[startSquare + 16].type == Type::Empty && squares[startSquare + 8].type == Type::Empty){
                Move move(startSquare, startSquare + 16);
                moves.push_back(move);
            }
            
        }
        // after moves (always can be done)
        if(squares[startSquare + 8].type == Type::Empty){
            Move move(startSquare, startSquare + 8);
            moves.push_back(move);
        }
        if((startSquare + 1) % 8){
            if(enPassantSquare == startSquare + 9 || (squares[startSquare + 9].type != Type::Empty && squares[startSquare + 9].color == Colors::White)){
                Move move(startSquare, startSquare + 9);
                moves.push_back(move);
            }
        }
        if(startSquare % 8){
            if(enPassantSquare == startSquare + 7 || (squares[startSquare + 7].type != Type::Empty && squares[startSquare + 7].color == Colors::White)){
                Move move(startSquare, startSquare + 7);
                moves.push_back(move);
            }
        }
        
    }
    else if(color == Colors::White){
        // Have to move in negative direction

        if(startSquare > 47){
            if(squares[startSquare - 16].type == Type::Empty && squares[startSquare - 8].type == Type::Empty){
                Move move(startSquare, startSquare - 16);
                moves.push_back(move);
            }
        }
            
        // after moves (always can be done)

        if(squares[startSquare - 8].type == Type::Empty){
            Move move(startSquare, startSquare - 8);
            moves.push_back(move);
        }
        if((startSquare + 1) % 8){
            if(enPassantSquare == startSquare - 7 || (squares[startSquare - 7].type != Type::Empty && squares[startSquare - 7].color == Colors::Black)){
                Move move(startSquare, startSquare - 7);
                moves.push_back(move);
            }
        }
        if(startSquare % 8){
            if(enPassantSquare == startSquare - 9 || (squares[startSquare - 9].type != Type::Empty && squares[startSquare - 9].color == Colors::Black)){
                Move move(startSquare, startSquare - 9);
                moves.push_back(move);
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

int Board::findKing(Colors& color){
    for (int pos = 0; pos < 64; pos++)
    {
        if(squares[pos].type == Type::King && squares[pos].color == color) return pos;
    }
    
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
    if(legalMoves.empty()){
        if(isSquareAttacked(findKing(turn), oppTurn)){
            if(turn == Colors::Black) gameState = GameState::WhiteWin;
            else if(turn == Colors::White) gameState = GameState::BlackWin;
        }
        else{
            gameState = GameState::Draw;
        }
    }
    return legalMoves;
}

void Board::MakeMove(Move move){

    //making sure enPassant remains for only 1 turn
    enPassantSquare = -1;

    // assuming its valid
    int startSquare = move.startSquare;
    
    int endSquare = move.endSquare;
    int oldRookSquare = move.oldRookSquare;
    int newRookSquare = move.newRookSquare;
    
    bool isCastle = move.isCastling;

    //setting up castling flags
    if(whiteCastleKingSide){
        if(startSquare == 63 || endSquare == 63 || (squares[startSquare].color == Colors::White && squares[startSquare].type == Type::King)){
            whiteCastleKingSide = false;
        }
    }
    if(whiteCastleQueenSide){
        if(startSquare == 56 || endSquare == 56 || (squares[startSquare].color == Colors::White && squares[startSquare].type == Type::King)){
            whiteCastleQueenSide = false;
        }
    }
    if(blackCastleKingSide){
        if(startSquare == 7 || endSquare == 7 || (squares[startSquare].color == Colors::Black && squares[startSquare].type == Type::King)){
            blackCastleKingSide = false;
        }
    }
    if(blackCastleQueenSide){
        if(startSquare == 0 || endSquare == 0 || (squares[startSquare].color == Colors::Black && squares[startSquare].type == Type::King)){
            blackCastleQueenSide = false;
        }
    }
    if(isCastle){
        squares[endSquare] = squares[startSquare];
        squares[newRookSquare] = squares[oldRookSquare];
        
        squares[startSquare].type = Type::Empty;
        squares[startSquare].color = Colors::None;
        squares[oldRookSquare].type = Type::Empty;
        squares[oldRookSquare].color = Colors::None;
    }
    else{
        if(squares[startSquare].type == Type::Pawn){
            if(turn == Colors::Black){

                //for setting up enPassant sq
                if(startSquare < 16 && endSquare == startSquare + 16){
                    enPassantSquare = startSquare + 8;
                }

                //for detecting if other player took the pawn using enPassant
                if((endSquare == startSquare + 7 || endSquare == startSquare + 9) && squares[endSquare].type == Type::Empty){
                    squares[endSquare - 8].type = Type::Empty;
                    squares[endSquare - 8].color = Colors::None;
                }
            } 
            else if(turn == Colors::White){

                //for setting up enPassant sq
                if(startSquare >= 48 && endSquare == startSquare - 16){
                    enPassantSquare = startSquare - 8;
                }

                //for detecting if other player took the pawn using enPassant
                if((endSquare == startSquare - 7 || endSquare == startSquare - 9) && squares[endSquare].type == Type::Empty){
                    squares[endSquare + 8].type = Type::Empty;
                    squares[endSquare + 8].color = Colors::None;
                }
            }
        }

        if(move.promotionPiece != Type::Empty){
            squares[endSquare].type = move.promotionPiece;
        }
        else {
            squares[endSquare].type = squares[startSquare].type;
        }
        squares[endSquare].color = squares[startSquare].color;

        squares[startSquare].type = Type::Empty;
        squares[startSquare].color = Colors::None;

    }
    turn = (Colors)(1 - (int)turn);
    history.push_back(move);
    GenerateMoves(); //just for instant checkmate and stalemate flags
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

Move Board::UciToMove(std::string uci){
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
    return move;    
}

std::string Board::MoveToUci(Move move){
    std::string uci = IndexToSquare(move.startSquare) + IndexToSquare(move.endSquare);    
    if(move.promotionPiece != Type::Empty){
        if(move.promotionPiece == Type::Queen) uci += 'q';
        else if(move.promotionPiece == Type::Rook) uci += 'r';
        else if(move.promotionPiece == Type::Bishop) uci += 'b';
        else if(move.promotionPiece == Type::Knight) uci += 'n';
    }
    
    return uci;
}
