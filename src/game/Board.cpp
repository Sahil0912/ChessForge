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
    
}

Piece Board::GetPiece(int index) const {
    return squares[index];
}

Colors Board::GetTurn() const {
    return turn;
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
            if(squares[startSquare + 9].type != Type::Empty && squares[startSquare + 9].color == Colors::White){
                Move move(startSquare, startSquare + 9);
                moves.push_back(move);
            }
        }
        if(startSquare % 8){
            if(squares[startSquare + 7].type != Type::Empty && squares[startSquare + 7].color == Colors::White){
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
            if(squares[startSquare - 7].type != Type::Empty && squares[startSquare - 7].color == Colors::Black){
                Move move(startSquare, startSquare - 7);
                moves.push_back(move);
            }
        }
        if(startSquare % 8){
            if(squares[startSquare - 9].type != Type::Empty && squares[startSquare - 9].color == Colors::Black){
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

std::vector<Move> Board::GenerateMoves(){
    std::vector<Move> moves;
    for(int i = 0; i < 64; i++){
        if(squares[i].color == turn){
            GeneratePieceMoves(i, squares[i].type, squares[i].color, moves);
        }
    }
    return moves;
}

void Board::MakeMove(Move move){
    // assuming its valid
    int startSquare = move.startSquare;
    
    int endSquare = move.endSquare;

    if(move.promotionPiece != Type::Empty){
        squares[endSquare].type = move.promotionPiece;
    }
    else {
        squares[endSquare].type = squares[startSquare].type;
    }
    squares[endSquare].color = squares[startSquare].color;

    squares[startSquare].type = Type::Empty;
    squares[startSquare].color = Colors::None;

    turn = (Colors)(1 - (int)turn);
}


/*
    Remaining stuff :
        Checks
        Pawn Promotion
        discovered check
        Castling
*/