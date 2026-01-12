#pragma once

enum class Type{
    Empty, Pawn, Knight, Bishop, Rook, Queen, King
};

enum class Colors{
    White, Black, None
};

typedef struct Piece_{
    Colors color;
    Type type;
}Piece;