#pragma once

enum class Type{
    Empty, Pawn, Knight, Bishop, Rook, Queen, King
};

enum class Colors{
    White = 0, Black = 1, None = 2
};

typedef struct Piece_{
    Colors color;
    Type type;
}Piece;